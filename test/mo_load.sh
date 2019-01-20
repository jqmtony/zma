#!/bin/sh
MyPath=$(cd $(dirname $0)/..; pwd)
cd $MyPath

. conf/globle.cfg
. bin/functions.sh

status=0
message="no message"

########################################################################
# load   负载状态监控
# hostname uptime proc_r proc_b load_1 load_5 load_15 date
########################################################################

self_check $server_name
echo $$ > var/run/$server_name.pid

##匹配一天以内重启时间的情况
msg=`uptime`
uptime=`echo $msg | sed -n 's/^\(.*\),.*user.*$/\1/p' | awk 'BEGIN{{ORS=""}}{for(i=3;i<=NF;i++) print $i" "}END{print "\n"}' | sed 's/,//g'`

loadmsg=`echo $msg | sed -n 's/^.*load average: \(.*\)$/\1/p' | sed 's/,//g'`
arr=($loadmsg)
load_1="${arr[0]}"
load_5="${arr[1]}"
load_15="${arr[2]}"

msg=`vmstat 10 -n 2| tail -n 1`
arr=($msg)
proc_r="${arr[0]}"
proc_b="${arr[1]}"

if [ `printf "%1.f\n" $load_1` -gt 15 ];then
        message="$mydate $hostname load is $load_1"
        status=1
fi

echo "{'hostname':$hostname,'uptime':$uptime,'date':$mydate,'status':$status,'proc_r':$proc_r,'proc_b':$proc_b,'load_1':$load_1,'load_5':$load_5,'load_15':$load_15,'msg':$message}"

rm -f var/run/$server_name.pid