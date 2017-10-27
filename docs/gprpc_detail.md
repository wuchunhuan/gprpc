# gprpc in detail
gprpc is a RPC(Remote Procedure Call) framework which can be used between distributed applications and services, the concept is simple: Client side application makes a RPC call to a service runing on a remote server as if it's calling a native method, all the dirty work including network trasmiting are taken care of by gprpc framework. Like other RPC system, user have to define a IDL file to generate a service interface which will be implemented by server application, then client side application can then call the stub interface to make a rpc call.

## Protocol between gprpc client and server
gprpc data exchange protocol is defined with a stantard proto file as follow:
```
package gprpc;

option cc_generic_services = true;

message rpc_data {
    enum type {
        REQUEST = 0;
        RESPONSE = 1;
    }
    type data_type = 1;
    string  svc_name = 2;
    string  method_name = 3;
    uint32  call_id = 4;

    bool    compress = 5;
    bool    encrypt = 6;

    bool    failed = 7;
    int32   error_code = 8;
    string  reason = 9;
    //cli ip
    //srv ip

    bytes   content = 10;
}
```
[gprpc.proto](src/proto/gprpc.proto)

| Field  | Meaning |
| ------------- | ------------- |
| `rpc_data`  | Data protocol structure name  |
| `type`  | Embeded definiton `REQUEST`: indicates a rpc request call<br>`ESPONSE`: indicates a rpc response |
| `data_type` | Value of `type` |
| `svc_name`| RPC service to be called in rpc client or called in server |
| `method_name` | RPC method of the above service to be called in rpc client or called in server |
| `call_id` | Unique ID represents an individual RPC call|
| `compress` | Not impemented yet, reserved |
| `encrypt` | Not impemented yet, reserved |
| `failed` | RPC client will check this field to distinguish a successful of failed RPC call |
| `error_code` | error code |
| `reason` | detailed reason for failed RPC call |
| `content` | Serialized request data of client or response data of server |

## RPC channel
RPC channel represents a communication line to a remote service which can be used to call that Service's
methods. A RPC channel is defined by remote server domain name(ip) and port, which can be utilized by different RPCs call to the same remote server. Besides that, A RPC channel alse encapsules user reqeust data with exchange protocol, and decapsulates and process response data.

## RPC controller
RPC controller is used to contorl how a rpc call should behave, like setting timer on a rpc call, enable compression and encryption, moreover, RPC controller can be used to retrive a RPC call status by client.

## Time out control
Time out can be set on both synchronous and asynchronous call, and it can be specified on different phase during a RPC call:
- **Total time out**: cntl->set_total_timeout(5000);//milliseconds
- **Resolving time out**: cntl->set_resolve_timeout(10);
- **Connecting time out**: cntl->set_connect_timeout(10);
- **Sending time out**: cntl->set_write_timeout(20);
- **Receiving time out**: cntl->set_read_timeout(20);
