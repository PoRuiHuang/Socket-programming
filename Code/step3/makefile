all: 
	g++ client.cpp -L /home/ebbandflow/桌面/internet/b06705027_part3/openssl-1.1.1d -l ssl -l crypto -o client
	g++ server.cpp -L /home/ebbandflow/桌面/internet/b06705027_part3/openssl-1.1.1d -l ssl -l crypto -o server -lpthread
	openssl genrsa -out payer_pri.pem 1024
	openssl rsa -in payer_pri.pem -pubout -out payer_pub.pem
	openssl genrsa -out payee_pri.pem 1024
	openssl rsa -in payee_pri.pem -pubout -out payee_pub.pem
clean:
	rm -f client
	rm -f server
	rm -f payer_pri.pem
	rm -f payer_pub.pem
	rm -f payee_pri.pem
	rm -f payee_pub.pem
	