#!/bin/bash

LOG_DIR=${ARTIFACT_ROOT}/reproduce_results/Figure14/tf
mkdir -p ${LOG_DIR}
cd ${ARTIFACT_ROOT}/../baseline
cd lstm
python3 lstm_tf.py --platform V100 --bs 1 2>&1 | tee ${LOG_DIR}/lstm.b1.log
python3 lstm_tf.py --platform V100 --bs 64 2>&1 | tee ${LOG_DIR}/lstm.b64.log
cd ..
cd nasrnn
python3 nas_tf.py --platform V100 --bs 1 2>&1 | tee ${LOG_DIR}/nasrnn.b1.log
python3 nas_tf.py --platform V100 --bs 64 2>&1 | tee ${LOG_DIR}/nasrnn.b64.log
cd ..
cd attention
python3 attention_tf.py --platform V100 --bs 1 2>&1 | tee ${LOG_DIR}/attention.b1.log
python3 attention_tf.py --platform V100 --bs 64 2>&1 | tee ${LOG_DIR}/attention.b64.log
cd ..
cd seq2seq
python3 seq2seq_tf.py --platform V100 --bs 1 2>&1 | tee ${LOG_DIR}/seq2seq.b1.log
python3 seq2seq_tf.py --platform V100 --bs 64 2>&1 | tee ${LOG_DIR}/seq2seq.b64.log
cd ..
cd blockdrop
python3 blockdrop_tf.py --platform V100 --bs 1 2>&1 | tee ${LOG_DIR}/blockdrop.b1.log
python3 blockdrop_tf.py --platform V100 --bs 64 2>&1 | tee ${LOG_DIR}/blockdrop.b64.log
cd ..
cd skipnet
python3 skipnet_tf.py --platform V100 --bs 1 2>&1 | tee ${LOG_DIR}/skipnet.b1.log
cd ..
