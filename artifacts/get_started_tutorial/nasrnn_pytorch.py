import torch
import torch.nn as nn
from time import time
import argparse
parser = argparse.ArgumentParser()
parser.add_argument('--bs', type=int, default=1)
arguments = parser.parse_args()

cuda_device = torch.device("cuda:0")
n_warmup = 100
n_run = 100

class NasRNN(nn.Module):
    def __init__(self, input_size, hidden_size):
        super(NasRNN, self).__init__()
        self.weight_ih = nn.Parameter(torch.randn(
            8, input_size, hidden_size, dtype=torch.float32))
        self.weight_hh = nn.Parameter(torch.randn(
            8, hidden_size, hidden_size, dtype=torch.float32))
        self.hidden_size = hidden_size
        nn.init.xavier_uniform_(self.weight_ih)
        nn.init.xavier_uniform_(self.weight_hh)

    def forward(self, inputs):  # seq_len, batch, input_size
        batch_size = inputs.shape[1]
        state_c = torch.ones(batch_size, self.hidden_size, device='cuda')
        state_m = torch.ones(batch_size, self.hidden_size, device='cuda')
        for i in range(inputs.size()[0]):
            inp = inputs[i]

            ih = torch.matmul(inp, self.weight_ih)
            hh = torch.matmul(state_m, self.weight_hh)

            i0 = ih[0]
            i1 = ih[1]
            i2 = ih[2]
            i3 = ih[3]
            i4 = ih[4]
            i5 = ih[5]
            i6 = ih[6]
            i7 = ih[7]

            h0 = hh[0]
            h1 = hh[1]
            h2 = hh[2]
            h3 = hh[3]
            h4 = hh[4]
            h5 = hh[5]
            h6 = hh[6]
            h7 = hh[7]

            layer1_0 = torch.sigmoid(i0 + h0)
            layer1_1 = torch.relu(i1 + h1)
            layer1_2 = torch.sigmoid(i2 + h2)
            layer1_3 = torch.relu(i3 + h3)
            layer1_4 = torch.tanh(i4 + h4)
            layer1_5 = torch.sigmoid(i5 + h5)
            layer1_6 = torch.tanh(i6 + h6)
            layer1_7 = torch.sigmoid(i7 + h7)

            l2_0 = torch.tanh(layer1_0 * layer1_1)
            l2_1 = torch.tanh(layer1_2 + layer1_3)
            l2_2 = torch.tanh(layer1_4 * layer1_5)
            l2_3 = torch.sigmoid(layer1_6 + layer1_7)

            # Inject the cell
            l2_0_v2 = torch.tanh(l2_0 + state_c)

            # Third layer
            state_c = l2_0_v2 * l2_1
            l3_1 = torch.tanh(l2_2 + l2_3)

            # Final layer
            state_m = torch.tanh(state_c * l3_1)

        return state_m


def test_model(enable_torch, batch_size, *params):
    input_size, hidden_size, seq_len = params
    model = NasRNN(input_size, hidden_size).cuda()
    model.eval()
    if enable_torch:
        model = torch.jit.script(model)
    embed = torch.randn([seq_len, batch_size, input_size], device=cuda_device)
    print("----batch_size={}---torchscript={}----".format(batch_size, enable_torch))
    print("[warmup]")
    torch.cuda.synchronize()
    for i in range(n_warmup):
        t0 = time()
        _ = model(embed)
        torch.cuda.synchronize()
        print("Time {} ms".format((time() - t0) * 1000))

    iter_times = []
    torch.cuda.synchronize()
    for i in range(n_run):
        start_time = time()
        _ = model(embed)
        torch.cuda.synchronize()
        iter_time = (time() - start_time) * 1000
        iter_times.append(iter_time)
    print("\033[31mSummary: [min, max, mean] = [%f, %f, %f] ms\033[m" % (
            min(iter_times), max(iter_times), sum(iter_times) / len(iter_times)))


if __name__ == '__main__':
    input_size = 256
    hidden_size = 256
    seq_len = 1000

    with torch.no_grad():
        test_model(True, arguments.bs, input_size, hidden_size, seq_len)