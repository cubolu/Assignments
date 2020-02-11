##!/bin/bash
# Author:   Jakub Lukac
# e-mail:   xlukac09@stud.fit.vutbr.cz

PORT=45687

PROJECT_DIR=`pwd`
SERVER_DIR=/test/server_files
CLIENT_DIR=/test

# run server in SERVER_DIR
cd "$PROJECT_DIR""$SERVER_DIR"
"$PROJECT_DIR"/server -p $PORT &
SERVER_PID="$!"

# run client in CLIENT_DIR
cd "$PROJECT_DIR""$CLIENT_DIR"

"$PROJECT_DIR"/client -h localhost -p $PORT -u big.txt
diff big.txt "$PROJECT_DIR""$SERVER_DIR"/big.txt > /dev/null 2> /dev/null
ret="$?"
if [ "$ret" -eq "0" ]; then
    echo "  big.txt: TEST SUCCESSED!"
else
    echo "  big.txt: TEST FAILED!"
fi

"$PROJECT_DIR"/client -h localhost -p $PORT -d FIT_image.eps
diff FIT_image.eps "$PROJECT_DIR""$SERVER_DIR"/FIT_image.eps > /dev/null 2> /dev/null
ret="$?"
if [ "$ret" -eq "0" ]; then
    echo "  FIT_image.eps: TEST SUCCESSED!"
else
    echo "  FIT_image.eps: TEST FAILED!"
fi

# kill server
kill -2 "$SERVER_PID"
rm -f "$PROJECT_DIR""$SERVER_DIR"/big.txt FIT_image.eps
