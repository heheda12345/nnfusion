
# parsetab.py
# This file is automatically generated. Do not edit.
# pylint: disable=W,C,R
_tabversion = '3.10'

_lr_method = 'LALR'

_lr_signature = "CHAR COMMENT1 COMMENT2 DPOUND ELSE FLOAT FOR GLOBAL ID IF INTEGER POUND QUALIFIER SHARED STRING SYNC TYPE VOID WSstart : signature \n             | shared\n    signature : GLOBAL VOID ID '(' parameters ')'parameters : parameter\n                  | parameters ',' parameter\n    parameter : type ID\n                 | type QUALIFIER ID\n                 | QUALIFIER type ID\n    type : TYPE \n            | TYPE '*'\n    shared : SHARED TYPE ID '[' INTEGER ']' ';' "
    
_lr_action_items = {'GLOBAL':([0,],[4,]),'SHARED':([0,],[5,]),'$end':([1,2,3,18,28,],[0,-1,-2,-3,-11,]),'VOID':([4,],[6,]),'TYPE':([5,10,15,19,],[7,16,16,16,]),'ID':([6,7,14,16,21,22,23,],[8,9,20,-9,26,27,-10,]),'(':([8,],[10,]),'[':([9,],[11,]),'QUALIFIER':([10,14,16,19,23,],[15,21,-9,15,-10,]),'INTEGER':([11,],[17,]),')':([12,13,20,25,26,27,],[18,-4,-6,-5,-7,-8,]),',':([12,13,20,25,26,27,],[19,-4,-6,-5,-7,-8,]),'*':([16,],[23,]),']':([17,],[24,]),';':([24,],[28,]),}

_lr_action = {}
for _k, _v in _lr_action_items.items():
   for _x,_y in zip(_v[0],_v[1]):
      if not _x in _lr_action:  _lr_action[_x] = {}
      _lr_action[_x][_k] = _y
del _lr_action_items

_lr_goto_items = {'start':([0,],[1,]),'signature':([0,],[2,]),'shared':([0,],[3,]),'parameters':([10,],[12,]),'parameter':([10,19,],[13,25,]),'type':([10,15,19,],[14,22,14,]),}

_lr_goto = {}
for _k, _v in _lr_goto_items.items():
   for _x, _y in zip(_v[0], _v[1]):
       if not _x in _lr_goto: _lr_goto[_x] = {}
       _lr_goto[_x][_k] = _y
del _lr_goto_items
_lr_productions = [
  ("S' -> start","S'",1,None,None,None),
  ('start -> signature','start',1,'p_start','cuparse.py',130),
  ('start -> shared','start',1,'p_start','cuparse.py',131),
  ('signature -> GLOBAL VOID ID ( parameters )','signature',6,'p_signature','cuparse.py',136),
  ('parameters -> parameter','parameters',1,'p_parameters','cuparse.py',141),
  ('parameters -> parameters , parameter','parameters',3,'p_parameters','cuparse.py',142),
  ('parameter -> type ID','parameter',2,'p_parameter','cuparse.py',147),
  ('parameter -> type QUALIFIER ID','parameter',3,'p_parameter','cuparse.py',148),
  ('parameter -> QUALIFIER type ID','parameter',3,'p_parameter','cuparse.py',149),
  ('type -> TYPE','type',1,'p_type','cuparse.py',159),
  ('type -> TYPE *','type',2,'p_type','cuparse.py',160),
  ('shared -> SHARED TYPE ID [ INTEGER ] ;','shared',7,'p_shared','cuparse.py',167),
]
