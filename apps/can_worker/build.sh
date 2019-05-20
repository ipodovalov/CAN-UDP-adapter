#!/bin/bash
# Cборка программ для платы МПМ или для обычного Linux
# ./build.sh (для МПМ)
# ./build.sh HOST (для HOST-машины c обычным Linux)
COUNT_STR_ERR=0

. ../../functions

if [ $# -lt 1 ]
then
	echo $0 используем адрес устройства по умолчанию \(10.0.2.214\)
	ADDRESS_DEVICE=10.0.2.214
	FLAG=1
else
	FLAG=0
fi

TARGET=$(basename `pwd`)
mkdir -p build
cd build
if [ "$FLAG" != 0 ]
then
	cmake -DCMAKE_TOOLCHAIN_FILE=../../cmake/buildroot.cmake .. 1> /dev/null
else
	cmake -DCMAKE_TOOLCHAIN_FILE=../../cmake/host.cmake .. 1> /dev/null
fi
make 2> error.tmp 1> /dev/null

COUNT_STR_ERR=$(wc -l error.tmp)

if [ "$COUNT_STR_ERR" != "0 error.tmp" ]
then
	echo_failure
	echo Ошибки сборки:
	cat error.tmp
	rm -f error.tmp
	make clean 1> /dev/null
	rm -r ../build
else
	echo Сборка программы $TARGET прошла  успешно
	echo_ok

	if [ "$FLAG" != 0 ]
	then
		echo остановим ${TARGET%.*} на устройстве
		ssh root@$ADDRESS_DEVICE "killall ${TARGET%.*}" &> /dev/null
		if [ $? -eq 0 ]; then
                	echo_ok
		else
			echo_failure
		fi

		echo отправляем файл программы на устройство
		scp ${TARGET%.*} root@$ADDRESS_DEVICE:/usr/sbin 1> /dev/null
		if [ $? -eq 0 ]; then
                	echo_ok
		else
			echo_failure
		fi

		echo отправляем также в папку с оверлеем \(внутри buildroot\)
#		cp ${TARGET%.*} ../../../../buildroot/system/overlay/usr/sbin/
		if [ $? -eq 0 ]; then
                	echo_ok
		else
			echo_failure
		fi
	else
		echo отправляем папку с оверлеем \(внутри проекта\)
		cp ${TARGET%.*} ../../../overlay/usr/sbin/
		if [ $? -eq 0 ]; then
                	echo_ok
		else
			echo_failure
		fi
	fi

	rm -f error.tmp
	rm -f $TARGET

	make clean 1> /dev/null
	cd ..
	rm -r build
fi
