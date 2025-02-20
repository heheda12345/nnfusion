"""Control flow graph analysis.

Given a Python AST we construct a doubly linked control flow graph whose nodes
contain the AST of the statements. We can then perform forward analysis on this
CFG.

"""
from __future__ import absolute_import
import functools
import operator
import astunparse

import gast

from . import annotations as anno
from . import ast_utils
from . import grammar

from collections import deque

class Node(object):
    """A node in the CFG."""
    __slots__ = ['next', 'value', 'prev']

    def __init__(self, value):
        self.next = set()
        self.prev = set()
        self.value = value


class CFG(gast.NodeVisitor):
    """Construct a control flow graph.

    Each statement is represented as a node. For control flow statements such
    as conditionals and loops the conditional itself is a node which either
    branches or cycles, respectively.

    Attributes:
        entry: The entry node, which contains the `gast.arguments` node of the
                function definition.
        exit: The exit node. This node is special because it has no value (i.e. no
                corresponding AST node). This is because Python functions can have
                multiple return statements.
    """

    def __init__(self):
        # The current leaves of the CFG
        self.head = []
        # A stack of continue statements
        self.continue_ = []
        # A stack of break nodes
        self.break_ = []
        self.nodes = {}


    @staticmethod
    def backlink(node):
        """Given a CFG with outgoing links, create incoming links."""
        seen = set()
        to_see = [node]
        while to_see:
            node = to_see.pop()
            seen.add(node)
            for succ in node.next:
                succ.prev.add(node)
                if succ not in seen:
                    to_see.append(succ)

    def set_head(self, node):
        """Link this node to the current leaves."""
        for head in self.head:
            head.next.add(node)
        self.head[:] = []
        self.head.append(node)

    @classmethod
    def build_cfg(cls, node):
        """Build a CFG for a function.

        Args:
            node: A function definition the body of which to analyze.

        Returns:
            A CFG object.

        Raises:
            TypeError: If the input is not a function definition.
        """
        if not isinstance(node, gast.FunctionDef):
            raise TypeError('input must be a function definition')
        cfg = cls()
        cfg.entry = Node(node.args)
        cfg.nodes[node.args] = cfg.entry
        cfg.head = [cfg.entry]
        cfg.visit_statements(node.body)
        cfg.exit = Node(None)
        cfg.set_head(cfg.exit)
        cfg.backlink(cfg.entry)
        return cfg

    def visit_statements(self, nodes):
        for node in nodes:
            if isinstance(node, grammar.CONTROL_FLOW):
                self.visit(node)
            else:
                expr = Node(node)
                self.nodes[node] = expr
                self.set_head(expr)

    def generic_visit(self, node):
        raise ValueError('unknown control flow')

    def visit_If(self, node):
        # The current head will hold the conditional
        test = Node(node.test)
        self.nodes[node.test] = test
        self.set_head(test)
        # Handle the body
        self.visit_statements(node.body)
        body_exit = self.head[:]
        self.head[:] = []
        self.head.append(test)
        # Handle the orelse
        self.visit_statements(node.orelse)
        self.head.extend(body_exit)

    def visit_While(self, node):
        test = Node(node.test)
        self.nodes[node.test] = test
        self.set_head(test)
        # Start a new level of nesting
        self.break_.append([])
        self.continue_.append([])
        # Handle the body
        self.visit_statements(node.body)
        self.head.extend(self.continue_.pop())
        self.set_head(test)
        # Handle the orelse
        self.visit_statements(node.orelse)
        # The break statements and the test go to the next node
        self.head.extend(self.break_.pop())

    def visit_For(self, node):
        iter_ = Node(node)
        self.nodes[node] = iter_
        self.set_head(iter_)
        self.break_.append([])
        self.continue_.append([])
        self.visit_statements(node.body)
        self.head.extend(self.continue_.pop())
        self.set_head(iter_)
        self.head.extend(self.break_.pop())

    def visit_Break(self, node):
        self.break_[-1].extend(self.head)
        self.head[:] = []

    def visit_Continue(self, node):
        self.continue_[-1].extend(self.head)
        self.head[:] = []

    def visit_Try(self, node):
        self.visit_statements(node.body)
        body = self.head
        handlers = []
        for handler in node.handlers:
            self.head = body[:]
            self.visit_statements(handler.body)
            handlers.extend(self.head)
        self.head = body
        self.visit_statements(node.orelse)
        self.head = handlers + self.head
        self.visit_statements(node.finalbody)


class Forward(object):
    """Forward analysis on CFG.

    Args:
        label: A name for this analysis e.g. 'active' for activity analysis. The
                AST nodes in the CFG will be given annotations 'name_in', 'name_out',
                'name_gen' and 'name_kill' which contain the incoming values, outgoing
                values, values generated by the statement, and values deleted by the
                statement respectively.
        gen: A function which takes the CFG node as well as a set of incoming
                values. It must return a set of newly generated values by the statement
                as well as a set of deleted (killed) values.
        op: Either the AND or OR operator. If the AND operator is used it turns
                into forward must analysis (i.e. a value will only be carried forward
                if it appears on all incoming paths). The OR operator means that
                forward may analysis is done (i.e. the union of incoming values will be
                taken).
    """

    def __init__(self, label, gen, op=operator.or_):
        self.gen = gen
        self.op = op
        self.out_label = label + '_out'
        self.in_label = label + '_in'
        self.gen_label = label + '_gen'
        self.kill_label = label + '_kill'
        self.label = label

    def visit(self, node):
        if node.value:
            if anno.hasanno(node.value, self.out_label):
                before = hash(anno.getanno(node.value, self.out_label))
            else:
                before = None
            preds = [anno.getanno(pred.value, self.out_label)
                     for pred in node.prev
                     if anno.hasanno(pred.value, self.out_label)]
            if preds:
                incoming = functools.reduce(self.op, preds[1:], preds[0])
            else:
                incoming = frozenset()
            anno.setanno(node.value, self.in_label, incoming, safe=False)
            gen, kill = self.gen(node, incoming)
            anno.setanno(node.value, self.gen_label, gen, safe=False)
            anno.setanno(node.value, self.kill_label, kill, safe=False)
            anno.setanno(node.value, self.out_label, (incoming - kill) | gen,
                         safe=False)
            if hash(anno.getanno(node.value, self.out_label)) != before:
                for succ in node.next:
                    self.visit(succ)
        else:
            preds = [anno.getanno(pred.value, self.out_label)
                     for pred in node.prev]
            self.exit = functools.reduce(self.op, preds[1:], preds[0])


class Backward(object):
    """Backward analysis on CFG.

    Args:
        label: A name for this analysis e.g. 'active' for activity analysis. The
                AST nodes in the CFG will be given annotations 'name_in', 'name_out',
                'name_gen' and 'name_kill' which contain the incoming values, outgoing
                values, values generated by the statement, and values deleted by the
                statement respectively.
        gen: A function which takes the CFG node as well as a set of incoming
                values. It must return a set of newly generated values by the statement
                as well as a set of deleted (killed) values.
        op: Either the AND or OR operator. If the AND operator is used it turns
                into forward must analysis (i.e. a value will only be carried forward
                if it appears on all incoming paths). The OR operator means that
                forward may analysis is done (i.e. the union of incoming values will be
                taken).
    """

    def __init__(self, label, gen, op=operator.or_):
        self.gen = gen
        self.op = op
        self.out_label = label + '_out'
        self.in_label = label + '_in'
        self.gen_label = label + '_gen'
        self.kill_label = label + '_kill'
    
    def visit(self, node):
        to_visit = deque([node])
        in_progress = set()
        in_progress.add(node)
        while len(to_visit) > 0:
            node = to_visit.popleft()
            in_progress.remove(node)
            if node.value:
                if anno.hasanno(node.value, self.out_label):
                    before = hash(anno.getanno(node.value, self.out_label))
                else:
                    before = None
                succs = [anno.getanno(succ.value, self.out_label)
                         for succ in node.next
                         if anno.hasanno(succ.value, self.out_label)]
                if succs:
                    incoming = functools.reduce(self.op, succs[1:], succs[0])
                else:
                    incoming = frozenset()
                anno.setanno(node.value, self.in_label, incoming, safe=False)
                gen, kill = self.gen(node, incoming)
                anno.setanno(node.value, self.gen_label, gen, safe=False)
                anno.setanno(node.value, self.kill_label, kill, safe=False)
                anno.setanno(node.value, self.out_label, (incoming - kill) | gen,
                             safe=False)
                if hash(anno.getanno(node.value, self.out_label)) != before:
                    for pred in node.prev:
                        if pred not in in_progress:
                            to_visit.append(pred)
                            in_progress.add(pred)
            else:
                for pred in node.prev:
                    if pred not in in_progress:
                        to_visit.append(pred)
                        in_progress.add(pred)

def forward(node, analysis):
    """Perform a given analysis on all functions within an AST."""
    if not isinstance(analysis, Forward):
        raise TypeError('not a valid forward analysis object')
    for succ in gast.walk(node):
        for label in [analysis.in_label, analysis.out_label, analysis.gen_label, analysis.kill_label]:
            if anno.hasanno(succ, label):
                anno.delanno(succ, label)
    for succ in gast.walk(node):
        if isinstance(succ, gast.FunctionDef):
            cfg_obj = CFG.build_cfg(succ)
            analysis.visit(cfg_obj.entry)
    return node


def backward(node, analysis):
    """Perform a given analysis on all functions within an AST."""
    if not isinstance(analysis, Backward):
        raise TypeError('not a valid backward analysis object')
    for succ in gast.walk(node):
        for label in [analysis.in_label, analysis.out_label, analysis.gen_label, analysis.kill_label]:
            if anno.hasanno(succ, label):
                anno.delanno(succ, label)
    cfg_nodes = {}
    for succ in gast.walk(node):
        if isinstance(succ, gast.FunctionDef):
            cfg_obj = CFG.build_cfg(succ)
            cfg_nodes.update(cfg_obj.nodes)
            analysis.visit(cfg_obj.exit)
    return cfg_nodes


def backward_block(stmt_list, analysis, names=None, rets=None):
    names = set()
    add_ret = False
    if not isinstance(stmt_list[-1], gast.Return):
        if rets is None:
            rets = set()
            for stmt in stmt_list:
                d = ast_utils.get_updated(stmt)
                rets.update(d)
        ret_str = "return " + ",".join(rets)
        ret_stmt = gast.parse(ret_str).body[0]
        stmt_list.append(ret_stmt)
        add_ret = True
        # print("ret_str", ret_str)

    if names is None:
        for stmt in stmt_list:
            for child in gast.walk(stmt):
                if isinstance(child, gast.Name):
                    names.add(child.id)

    func_node = gast.FunctionDef(
        name='f',
        args=gast.arguments(
            args=[gast.Name(id=name, ctx=gast.Param(), annotation=None, type_comment=None) for name in names],
            posonlyargs=[], vararg=None, kwonlyargs=[], kw_defaults=[], kwarg=None, defaults=[]
        ),
        body=stmt_list,
        decorator_list=[], returns=None, type_comment=None
    )

    new_node = backward(func_node, analysis)
    if add_ret:
        stmt_list.pop()
        return new_node.body[:-1]
    else:
        return new_node.body


class ReachingDefinitions(Forward):
    """Perform reaching definition analysis.

    Each statement is annotated with a set of (variable, definition) pairs.

    """

    def __init__(self):
        def definition(node, incoming):
            definitions = ast_utils.get_updated(node.value)
            gen = frozenset((id_, node.value) for id_ in definitions)
            kill = frozenset(def_ for def_ in incoming
                             if def_[0] in definitions)
            return gen, kill
        super(ReachingDefinitions, self).__init__('definitions', definition)


class Defined(Forward):
    """Perform defined variable analysis.

    Each statement is annotated with a set of variables which are guaranteed to
    be defined at that point.
    """

    def __init__(self):
        def defined(node, incoming):
            gen = ast_utils.get_updated(node.value)
            return gen, frozenset()
        super(Defined, self).__init__('defined', defined, operator.and_)


class Active(Forward):
    """Active variable analysis.

    Given a set of active arguments, find all variables that are active i.e.
    variables whose values possibly depend on the given set of arguments.

    Args:
        wrt: A tuple of indices of arguments that are active.
    """

    def __init__(self, wrt):
        def active(node, incoming):
            gen = set()
            kill = set()
            if isinstance(node.value, gast.arguments):
                gen.update(node.value.args[i].id for i in wrt)
            if isinstance(node.value, gast.Assign):
                # TODO: joint analysis of forward and backward
                for succ in gast.walk(node.value.value):
                    if isinstance(succ, gast.Name) and succ.id in incoming:
                        gen.update(ast_utils.get_updated(node.value))
                        break
                    elif ast_utils.is_attr_of(succ, 'self'):
                        gen.update(ast_utils.get_updated(node.value))
                        break
                    else:
                        kill.update(ast_utils.get_updated(node.value))
            return gen, kill
        super(Active, self).__init__('active', active)


class Invariant(Forward):
    # rely on the result of reaching definition
    def __init__(self):
        def invariant(node, incoming):
            gen = set()
            kill = set()
            defs = [id_ for id_, _ in anno.getanno(
                node.value, 'definitions_in')]

            if isinstance(node.value, gast.arguments):
                gen.add("torch")
                gen.update(arg.id for arg in node.value.args)
            if isinstance(node.value, gast.Assign):
                all_const = True
                for succ in gast.walk(node.value.value):
                    if isinstance(succ, gast.Name):
                        if succ.id not in incoming or defs.count(succ.id) > 1:
                            all_const = False
                upds = ast_utils.get_updated(node.value)

                for target in node.value.targets:
                    for succ in gast.walk(target):
                        if isinstance(succ, gast.Name):
                            if succ.id not in upds and (succ.id not in incoming or defs.count(succ.id) > 1):
                                all_const = False
                if all_const:
                    gen.update(upds)
                else:
                    kill.update(upds)
            return gen, kill

        super(Invariant, self).__init__('invariant', invariant, operator.and_)


def get_def_use(node):
    def_ = set()
    use = set()
    if isinstance(node, gast.arguments):
        def_.update(arg.id for arg in node.args)
    elif isinstance(node, gast.Assign):
        definitions = ast_utils.get_updated(node)
        def_.update(definitions)
        for succ in gast.walk(node.value):
            if isinstance(succ, gast.Name):
                use.add(succ.id)
    elif isinstance(node, gast.For):
        for succ in gast.walk(node.iter):
            if isinstance(succ, gast.Name):
                use.add(succ.id)
        def_.update(ast_utils.get_updated(node))
    elif isinstance(node, gast.If):
        for succ in gast.walk(node.test):
            if isinstance(succ, gast.Name):
                use.add(succ.id)
    else:
        for succ in gast.walk(node):
            if isinstance(succ, gast.Name):
                use.add(succ.id)
    return def_, use


class BackwardActive(Backward):
    def __init__(self):
        def bwd_active(node, incoming):
            gen = set()
            kill = set()
            if isinstance(node.value, gast.arguments):
                kill.update(arg.id for arg in node.value.args)
            elif isinstance(node.value, gast.Assign):
                definitions = ast_utils.get_updated(node.value)
                kill.update(definitions)
                is_active = False
                for d in definitions:
                    if d in incoming:
                        is_active = True
                if is_active:
                    for succ in gast.walk(node.value):
                        if isinstance(succ, gast.Name) and isinstance(succ.ctx, gast.Load):
                            gen.add(succ.id)
            elif isinstance(node.value, gast.For):
                for succ in gast.walk(node.value.iter):
                    if isinstance(succ, gast.Name):
                        gen.add(succ.id)
                kill.update(ast_utils.get_updated(node.value))
            elif isinstance(node.value, gast.While):
                for succ in gast.walk(node.value.test):
                    if isinstance(succ, gast.Name):
                        gen.add(succ.id)
            elif isinstance(node.value, gast.If):
                for succ in gast.walk(node.value.test):
                    if isinstance(succ, gast.Name):
                        gen.add(succ.id)
            else:
                for succ in gast.walk(node.value):
                    if isinstance(succ, gast.Name):
                        gen.add(succ.id)

            return gen, kill
        super(BackwardActive, self).__init__('bwd_active', bwd_active)


class ZeroTensor(Forward):
    def __init__(self):
        def zero_tensor(node, incoming):
            gen = set()
            kill = set()
            definitions = ast_utils.get_updated(node.value)
            kill.update(definitions)
            if isinstance(node.value, gast.Assign) and isinstance(node.value.value, gast.Call):
                func = node.value.value.func
                if isinstance(func, gast.Attribute) and isinstance(func.value, gast.Name) and func.value.id == 'torch' and (func.attr == 'zeros' or func.attr == 'zeros_like'):
                    gen.update(definitions)
            return gen, kill

        super(ZeroTensor, self).__init__('zero_tensor', zero_tensor, op=operator.and_)
