Documentación del proyecto: Instrucciones para el profesor Jesús Larez

Ante cualquier percanse al momento de la evaluación llamar al:
Luis Somoza: +58 4122155879
César Sotillo: +58 4121980578 (llamada) y +58 4165881300 (WhatsApp)

- Carpeta del proyecto: “Experimento 616”

Dentro de esta carpeta se puede visualizar:

	1) Archivo del broker mqtt: mqtt_tcp_server.cpp

		para su compilaciones hacer el siguiente comando: g++ -pthread mqtt_tcp_server.cpp -o server ----> compilador

	2) dentro de la carpeta “Experimento 616” se encuentra la carpeta PubSub que contiene los archivos de publicador y subscriptor implementados con paho eclipse.

	3) Dentro de la carpeta PubSub para compilar los archivos de publisher y subscriber se deben realizar los siguientes pasos que también estará en los archivos “.cpp” en caso de no leer este documento:

 		3.1) instalar la librería de Paho con el siguiente comando: sudo apt-get install libmqttclient-dev

		3.2) entrar entrar en la carpeta PubSub mediante la terminal con el comando: “cd PubSub”

		3.3) compilar el archivo a utilizar mediante el siguiente comando:

 			3.3.1) Publisher: g++ publisher.cpp -lpaho-mqtt3c -o publisher


			3.3.2) Subscriber: g++ subscriber.cpp -lpaho-mqtt3c -o  subscriber
