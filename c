#!/usr/bin/env sh


VBM=VBoxManage
VMID=8f8d7a7e-7aba-466f-a28f-3cf87100a58e
start(){
    $VBM startvm $VMID --type headless
}

stop() {
    $VBM controlvm $VMID poweroff
}

do_ssh(){
    ssh -p 2022 user@localhost $1
}

case $1 in
    start | up) start;;
    pause | suspend) pause;;
    stop | halt | down) stop;;
    ssh) shift; do_ssh "$@";;
    *) echo "Usage $0 {start|up|stop|halt|ssh}"; exit 1
esac
