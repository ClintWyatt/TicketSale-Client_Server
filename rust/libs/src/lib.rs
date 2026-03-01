use chrono::prelude::*;
use log::{info, warn};

extern crate flatbuffers;

// import the generated code
#[allow(dead_code, unused_imports)]
#[path = "./fbsFiles/ticket_client_generated.rs"]
pub mod ticket_client_generated;

use crate::ticket_client_generated::ClientInfo;
use crate::ticket_client_generated::ClientInfoArgs;
use crate::ticket_client_generated::HelloWorld;
use crate::ticket_client_generated::HelloWorldArgs;
use crate::ticket_client_generated::MessageUnion;
use crate::ticket_client_generated::RootTable;
use crate::ticket_client_generated::RootTableArgs;
use crate::ticket_client_generated::ScalperInfo;
use crate::ticket_client_generated::ScalperInfoArgs;
use crate::ticket_client_generated::TicketSale;
use crate::ticket_client_generated::TicketSaleArgs;
use flatbuffers::InvalidFlatbuffer;

pub fn serialize_hello_world() -> Vec<u8> {
    let mut builder = flatbuffers::FlatBufferBuilder::with_capacity(1024);
    let hello_world_str = builder.create_string("Hello world!");

    let now_utc: DateTime<Utc> = Utc::now();
    let time_stamp = now_utc.timestamp();

    let hello_world_args = HelloWorldArgs {
        message: Some(hello_world_str),
        timestamp: time_stamp,
    };

    let hello_world_offset = HelloWorld::create(&mut builder, &hello_world_args);

    let root_table_args = RootTableArgs {
        data_type: MessageUnion::HelloWorld,
        data: Some(hello_world_offset.as_union_value()),
    };

    let root_table_offset = RootTable::create(&mut builder, &root_table_args);
    builder.finish(root_table_offset, None);
    let buffer = builder.finished_data().to_vec();
    buffer
}

pub fn serialize_scalper_info(ip_addresses: &Vec<String>) -> Vec<u8> {
    let mut builder = flatbuffers::FlatBufferBuilder::with_capacity(1024);

    let string_offsets: Vec<_> = ip_addresses
        .iter()
        .map(|s| builder.create_string(s))
        .collect();
    let ip_address_vector = builder.create_vector(&string_offsets);

    let scalper_info_args = ScalperInfoArgs {
        scalper_address: Some(ip_address_vector),
    };

    let scalper_info_offset = ScalperInfo::create(&mut builder, &scalper_info_args);

    let root_table_args = RootTableArgs {
        data_type: MessageUnion::ScalperInfo,
        data: Some(scalper_info_offset.as_union_value()),
    };

    let root_table_offset = RootTable::create(&mut builder, &root_table_args);
    builder.finish(root_table_offset, None);
    let buffer = builder.finished_data().to_vec();
    buffer
}

pub fn serialize_ticket_sale(funds: i16, ticket_num: i16) -> Vec<u8> {
    let mut builder = flatbuffers::FlatBufferBuilder::with_capacity(1024);
    let ticket_sale_args = TicketSaleArgs {
        money: funds,
        insuffieient_funds: false,
        ticket_number: ticket_num,
    };
    let ticket_sale_offset = TicketSale::create(&mut builder, &ticket_sale_args);

    let root_table_args = RootTableArgs {
        data_type: MessageUnion::TicketSale,
        data: Some(ticket_sale_offset.as_union_value()),
    };

    let root_table_offset = RootTable::create(&mut builder, &root_table_args);
    builder.finish(root_table_offset, None);
    let buffer = builder.finished_data().to_vec();
    buffer
}

pub fn serialize_client_info(ip_address: &String, port: u16) -> Vec<u8> {
    let mut builder = flatbuffers::FlatBufferBuilder::with_capacity(1024);
    let client_ip = builder.create_string(ip_address);

    let client_info_args = ClientInfoArgs {
        ip_address: Some(client_ip),
        udp_port: port,
    };

    let client_info_offset = ClientInfo::create(&mut builder, &client_info_args);

    //let union = ClientInfo{client_info_offset};

    let root_table_args = RootTableArgs {
        data_type: MessageUnion::ClientInfo,
        data: Some(client_info_offset.as_union_value()),
    };

    let root_table_offset = RootTable::create(&mut builder, &root_table_args);
    builder.finish(root_table_offset, None);
    let buffer = builder.finished_data().to_vec();
    buffer
}

pub fn deserialize_hello_world(
    flatbuffer_data: Result<RootTable<'_>, InvalidFlatbuffer>,
) -> Option<HelloWorld<'_>> {
    let hello_world = flatbuffer_data
        .clone()
        .expect("Failed to get initial hello world object");
    let mut hello_world_return = hello_world.data_as_hello_world();
    match flatbuffer_data {
        Ok(root_data) => {
            let inner_message = flatbuffer_data.expect("Failed to get buffer data");
            //TODO -> need to use data_as_hello_world at the beginning of the method so that
            //We can return a Hello World object
            hello_world_return = inner_message.data_as_hello_world();
            match hello_world_return {
                Some(hello) => {
                    println!("Received hello world message.");
                    println!(
                        "Message: {:?} - time stamp {}",
                        hello.message(),
                        hello.timestamp()
                    );
                }
                None => {
                    println!("Failed to hello world message");
                }
            }
        }
        Err(_) => {
            println!("Error occured while deserializing hello world message");
        }
    }
    hello_world_return
}

pub fn verify_root_table(
    buffer: &[u8],
) -> (Result<RootTable<'_>, InvalidFlatbuffer>, MessageUnion) {
    let flatbuffer_root = flatbuffers::root::<RootTable>(buffer);
    let mut message_type = MessageUnion::NONE;

    //match flatbuffers::root::<RootTable>(buffer) {
    match flatbuffer_root {
        Ok(root_data) => match root_data.data_type() {
            MessageUnion::ClientInfo => {
                message_type = MessageUnion::ClientInfo;
                /*
                let inner_message =
                    flatbuffer_root.expect("Failed to get buffer data");
                let client_info = inner_message.data_as_client_info();
                match client_info {
                    Some(client) => {
                        println!("received client info message");
                        println!(
                            "Ip address: {:?} - port: {}",
                            client.ip_address(),
                            client.udp_port()
                        );
                    }
                    None => {
                        println!("Failed to parse client info message");
                    }
                }
                */
            }
            MessageUnion::TicketSale => {
                message_type = MessageUnion::TicketSale;
                /*
                let inner_message =
                    flatbuffer_root.expect("Failde to get buffer data");
                let ticket_info = inner_message.data_as_ticket_sale();
                match ticket_info {
                    Some(ticket) => {
                        println!("received ticket sale message");
                        println!(
                            "Funds: {} - can buy: {} - ticket number: {}",
                            ticket.money(),
                            ticket.insuffieient_funds(),
                            ticket.ticket_number()
                        );
                    }
                    None => {
                        println!("Failed to parse ticket sale message");
                    }
                }
                */
            }
            MessageUnion::ScalperInfo => {
                message_type = MessageUnion::ScalperInfo;
                /*
                let inner_message =
                    flatbuffer_root.expect("Failed to get buffer data");
                let scalper_info = inner_message.data_as_scalper_info();
                match scalper_info {
                    Some(scalper) => {
                        println!("received scalper info message");
                        println!("Ip addresses: {:?}", scalper.scalper_address());
                    }
                    None => {
                        println!("Failed to parse ticket sale message");
                    }
                }
                */
            }
            MessageUnion::HelloWorld => {
                message_type = MessageUnion::HelloWorld;
            }
            MessageUnion::ResetTickets => {
                message_type = MessageUnion::HelloWorld;
                /*
                let inner_message =
                    flatbuffer_root.expect("Failde to get buffer data");
                let client_info = inner_message.data_as_reset_tickets();
                println!("received reset tickets message");
                */
            }
            MessageUnion::NONE => {
                println!("received unknown message message");
            }
            _ => {
                println!("Received invalid enum");
            }
        },
        Err(ref error) => {
            println!("Failed to parse message! {:?}", error);
        }
    }

    (flatbuffer_root, message_type)
}

#[cfg(test)]
mod tests {
    use super::*;
    use more_asserts::assert_ge;

    #[test]
    fn serialize() {
        let result = serialize_hello_world();
        info!("Buffer is {:?}", result);
        assert_ne!(result.len(), 0);
    }

    #[test]
    fn deserialize() {
        let time_stamp = Utc::now().timestamp();

        let result = serialize_hello_world();
        //convert the bytes to a zmq message
        let message: zmq::Message = (&result[..]).into();
        verify_root_table(&message);

        //let hello_world = deserialize_hello_world(&message);
        //assert_eq!(hello_world.message(), Some("Hello world!"));
        //assert_ge!(hello_world.timestamp(), time_stamp);
    }

    #[test]
    fn test_verify_flat_buffers() {
        let ip_address = "192.168.0.1".to_string();
        let port = 12345;
        let buffer = serialize_client_info(&ip_address, port);

        let (root_buffer, message_type) = verify_root_table(&buffer);
        assert_eq!(message_type, MessageUnion::ClientInfo);

        let buffer = serialize_hello_world();
        let (root_buffer, message_type) = verify_root_table(&buffer);
        assert_eq!(message_type, MessageUnion::HelloWorld);

        //TODO - need to create a method that will return
        //the type of message that could be used, like a template.
        //note that the root_buffer is valid if any of the message
        //union types (except NONE). Just need to use the following line
        //        let inner_message =
        //            flatbuffer_root.expect("Failed to get buffer data");
        match message_type {
            MessageUnion::ClientInfo => {}
            MessageUnion::HelloWorld => {
                let msg = deserialize_hello_world(root_buffer);
                match msg {
                    Some(hello) => {
                        assert_eq!(hello.message(), Some("Hello world!"));
                    }
                    None => {
                        assert_eq!(1, 2);
                    }

                }
            }
            MessageUnion::ResetTickets => {}
            MessageUnion::TicketSale => {}
            MessageUnion::ScalperInfo => {}
            MessageUnion::NONE => {}
            _ => {}
        }

        let ip_addresses: Vec<String> = [
            "192.168.0.1:5555".to_string(),
            "192.168.0.2:5556".to_string(),
        ]
        .to_vec();
        let buffer = serialize_scalper_info(&ip_addresses);
        let (root_buffer, message_type) = verify_root_table(&buffer);
        assert_eq!(message_type, MessageUnion::ScalperInfo);

        let buffer = serialize_ticket_sale(20, 30);
        let (root_buffer, message_type) = verify_root_table(&buffer);
        assert_eq!(message_type, MessageUnion::TicketSale);
    }
}
