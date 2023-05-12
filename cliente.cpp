#include <stdio.h>
#include <assert.h>
#include <zmq.h>

int main()
{
    void *context = zmq_ctx_new();

    //Genero el Socket para hablar con el cliente
    void *cliente = zmq_socket(context, ZMQ_PUB);
    int rc = zmq_bind(cliente, "tcp://127.0.0.1:5556");
    assert(rc == 0);

    while(1)
    {
        rc = zmq_send(cliente, "Hello World!", 12, 0);
        assert(rc == 12);
    }

    zmq_close(cliente);
    zmq_ctx_destroy(context);

    return 0;
}