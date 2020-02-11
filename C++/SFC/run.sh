#!/usr/bin/env bash

BUILD_DIR='build/'
DATA_DIR='data/'
OUTPUT_APP='rbm'
echo "Downloading data set to directory \"$(pwd)/${DATA_DIR}\"..."
mkdir "${DATA_DIR}" 2> /dev/null

MNIST_URL='http://yann.lecun.com/exdb/mnist/'
TRAINING_FILE_NAME='train-images-idx3-ubyte.gz'
TEST_FILE_NAME='t10k-images-idx3-ubyte.gz'

TRAINING_FILE="${DATA_DIR}${TRAINING_FILE_NAME}"
TEST_FILE="${DATA_DIR}${TEST_FILE_NAME}"

curl -sS "${MNIST_URL}${TRAINING_FILE_NAME}" > "${TRAINING_FILE}"       && \
gzip -d "${TRAINING_FILE}"                                              && \
curl -sS "${MNIST_URL}${TEST_FILE_NAME}" > "${TEST_FILE}"               && \
gzip -d "${TEST_FILE}"                                                  && \
echo "Download of training and testing data set SUCCESSFUL."

if [[ "$1" = "cmake" ]]; then
  mkdir "${BUILD_DIR}" 2> /dev/null
  cd "${BUILD_DIR}"                                                     && \
  cmake -D CMAKE_BUILD_TYPE=Release ..                                  && \
  make                                                                  && \
  mv ${OUTPUT_APP} ../                                                  && \
  cd ..                                                                 && \
  "./rbm" -h
else
  make
  "./rbm" -h
fi
