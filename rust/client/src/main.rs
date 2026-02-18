use log::{info};

fn main() {
    let context = zmq::Context::new();
    /*
     * A socket of type 'ZMQ_REQ' is used by a client to send requests 
     * to and receive replies from a service. This socket type allows only 
     * an alternating sequence of zmq_send(request) and subsequent zmq_recv(reply) 
     * calls. Each request sent is round-robined among all services, 
     * and each reply received is matched with the last issued request.
     *
     * For connection-oriented transports, If the ZMQ_IMMEDIATE option is set and 
     * there is no service available, then any send operation on the socket 
     * shall block until at least one service becomes available. 
     * The REQ socket shall not discard messages.
     */

    let requester = context.socket(zmq::REQ).unwrap();

    assert!(requester.connect("tcp://localhost:5555").is_ok());

    //let mut msg = zmq::Message::new();
    let mut buffer: Vec<u8> = Vec::new();

    for request_numb in 1..=10 {
        if request_numb < 2 {
           buffer = generated::serialize_hello_world();
           println!("Sending Hello message");
        }
        else if request_numb < 4 {
            let ip_address = "192.168.0.1".to_string();
            let port_number = 5555;
           buffer = generated::serialize_client_info(&ip_address, port_number);
           println!("Sending client info");
        }
        else if request_numb < 6 {
           buffer = generated::serialize_ticket_sale(20, 30);
           println!("Sending ticket sale");
        }
        else {
           let ip_addresses: Vec<String> = ["192.168.0.1:5555".to_string(), "192.168.0.2:5556".to_string()].to_vec();
           buffer = generated::serialize_scalper_info(&ip_addresses);
           println!("Sending scalper mesage");
        }
        let send_result = requester.send(buffer, 0);

        match send_result {
            Ok(()) => {
                info!("Sent hello world message");
            }
            Err(e) => {
                println!("Failde to send message: {:?}", e);
            }
        }

        //requester.recv(&mut msg, 0).unwrap();
        let received_msg = requester.recv_bytes(0).expect("Failed to receive");
        generated::verify_root_table(&received_msg);
        /*
        println!("received message: {:?}", Some(received_msg.message()));
        println!("received timestamp: {}", received_msg.timestamp());
        */
    }
}
