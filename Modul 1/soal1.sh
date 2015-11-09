#!/bin/bash

echo -e "\n  PRAKTIKUM 1 - SISTEM OPERASI E"
echo -e "   Oleh:\n   5114100075 - Panji Rimawan\n   5114100100 - Steven Kurniawan"

pilihan=0
while [ "$pilihan" -ne 5 ]; do
	echo -e "\nPilih salah satu perintah yang ingin Anda eksekusi."
	echo " 1. Tampilkan Channel dan SSID dari WiFi di sekitar."
	echo " 2. Hitung jumlah proses dengan nama tertentu yang aktif."
	echo " 3. Hitung jumlah open file yang dibuka oleh suatu proses."
	echo " 4. Install Apache & tampilkan log."
	echo " 5. Keluar program."
	echo -n "Masukkan pilihan >> "
	read pilihan

	if [ $pilihan = 1 ]; then
		sudo iwlist wlan0 scan | grep -E "Channel:|ESSID:"
	elif [ $pilihan = 2 ]; then
		echo -n "Masukkan nama proses : "
		read process
		echo -n "Jumlah $process yang aktif : "
		count=`ps aux | grep "$process" | wc -l`
		let count=count-1
		echo $count
	elif [ $pilihan = 3 ]; then
		echo -n -e "Masukkan nama proses\t\t: "
		read proses
		pid=`ps aux | grep "$proses" | awk '{ print $2 }'`
		echo -n -e "Open file yang sedang diproses\t: "
		total=0
		for i in $pid; do
			count=`lsof -p $i | wc -l`
			let total=total+count
		done
		echo $total
	elif [ $pilihan = 4 ]; then
		echo "Memasang apache..."
		sudo apt-get updates
		sudo apt-get install apache2
		echo -e "\nMembuka syslog..."
		file=/var/log/syslog
		awk '{ print $1 " " $2 }' $file | more
	elif [ $pilihan -lt 1 ] || [ $pilihan -gt 5 ]; then
		echo -e "Input salah!! Silahkan coba lagi!"
	fi
done

echo -e "Program Exit.\n"
