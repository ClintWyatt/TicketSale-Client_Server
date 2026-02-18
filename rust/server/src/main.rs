
use std::thread;
use std::time::Duration;
use log::{info, warn};
//use generated::helloworld_generated;

fn main() {
    let context = zmq::Context::new();
    /*
     * A socket of type 'ZMQ_REP' is used by a service to receive requests 
     * from and send replies to a client. This socket type allows only an 
     * alternating sequence of zmq_recv(request) and subsequent zmq_send(reply) calls. 
     * Each request received is fair-queued from among all clients, and each reply sent is
     * routed to the client that issued the last request. 
     * If the original requester does not exist any more the reply is silently discarded.
     */
    let responder = context.socket(zmq::REP).unwrap();

    assert!(responder.bind("tcp://*:5555").is_ok());

    //let mut msg = zmq::Message::new();
    let mut buffer: Vec<u8> = Vec::new();
    loop {
        let received_msg = responder.recv_bytes(0).expect("Failed to receive");
        generated::verify_root_table(&received_msg); 

        thread::sleep(Duration::from_millis(1000));

        let buffer = generated::serialize_hello_world();
        responder.send(buffer, 0).unwrap();
    }
}
