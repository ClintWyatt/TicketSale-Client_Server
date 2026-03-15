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
use crate::ticket_client_generated::ResetTickets;
use crate::ticket_client_generated::ResetTicketsArgs;
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

pub fn serialize_reset_tickets(reset_event_flag: bool) -> Vec<u8> {
    let mut builder = flatbuffers::FlatBufferBuilder::with_capacity(1024);

    let reset_tickets_args = ResetTicketsArgs {
        reset_event: reset_event_flag,
    };

    let reset_ticket_offset = ResetTickets::create(&mut builder, &reset_tickets_args);

    let root_table_args = RootTableArgs {
        data_type: MessageUnion::ResetTickets,
        data: Some(reset_ticket_offset.as_union_value()),
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
        port: port,
    };

    let client_info_offset = ClientInfo::create(&mut builder, &client_info_args);

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
    let mut hello_world_return = flatbuffer_data
        .clone()
        .expect("Failed to get flatbuffer data")
        .data_as_hello_world();
    match flatbuffer_data {
        Ok(_) => {
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

pub fn deserialize_ticket_sale(
    flatbuffer_data: Result<RootTable<'_>, InvalidFlatbuffer>,
) -> Option<TicketSale<'_>> {
    let mut ticket_sale_return = flatbuffer_data
        .clone()
        .expect("Failed to get flatbuffer data")
        .data_as_ticket_sale();
    match flatbuffer_data {
        Ok(_) => {
            let inner_message = flatbuffer_data.expect("Failed to get buffer data");
            ticket_sale_return = inner_message.data_as_ticket_sale();
            match ticket_sale_return {
                Some(ticket_sale) => {
                    println!("Ticket sale message.");
                    println!(
                        "Money {} || funds {} || ticket number {}",
                        ticket_sale.money(),
                        ticket_sale.insuffieient_funds(),
                        ticket_sale.ticket_number()
                    );
                }
                None => {
                    println!("Failed to get ticket sale message");
                }
            }
        }
        Err(_) => {
            println!("Error occured while deserializing ticket sale message");
        }
    }
    ticket_sale_return
}

pub fn deserialize_scalper_info(
    flatbuffer_data: Result<RootTable<'_>, InvalidFlatbuffer>,
) -> Option<ScalperInfo<'_>> {
    let mut scalper_info_return = flatbuffer_data
        .clone()
        .expect("Failed to get flatbuffer data")
        .data_as_scalper_info();
    match flatbuffer_data {
        Ok(_) => {
            let inner_message = flatbuffer_data.expect("Failed to get buffer data");
            scalper_info_return = inner_message.data_as_scalper_info();
            match scalper_info_return {
                Some(scalper) => {
                    println!("scalper info message.");
                    println!("ip addressed: {:?}", scalper.scalper_address());
                }
                None => {
                    println!("Failed to get scalper info message");
                }
            }
        }
        Err(_) => {
            println!("Error occured while deserializing scalper info message");
        }
    }
    scalper_info_return
}

pub fn deserialize_reset_tickets(
    flatbuffer_data: Result<RootTable<'_>, InvalidFlatbuffer>,
) -> Option<ResetTickets<'_>> {
    let mut reset_tickets_return = flatbuffer_data
        .clone()
        .expect("Failed to get flatbuffer data")
        .data_as_reset_tickets();
    match flatbuffer_data {
        Ok(_) => {
            let inner_message = flatbuffer_data.expect("Failed to get buffer data");
            reset_tickets_return = inner_message.data_as_reset_tickets();
            match reset_tickets_return {
                Some(reset_tickets) => {
                    println!("Ticket reset message.");
                    println!("reset event: {:?}", reset_tickets.reset_event());
                }
                None => {
                    println!("Failed to get reset tickets message");
                }
            }
        }
        Err(_) => {
            println!("Error occured while deserializing reset tickets message");
        }
    }
    reset_tickets_return
}

pub fn deserialize_client_info(
    flatbuffer_data: Result<RootTable<'_>, InvalidFlatbuffer>,
) -> Option<ClientInfo<'_>> {

    let mut client_info_return: Option<ClientInfo> = None;
    match flatbuffer_data.clone() {
        Ok(inner_message) => {
            client_info_return = inner_message.data_as_client_info();
            match client_info_return {
                Some(client_info) => {
                    println!("Client info message.");
                    println!("Ip address: {:?}", Some(client_info.ip_address()));
                    println!("Port number: {}", client_info.port());
                }
                None => {
                    println!("Failed to get reset tickets message");
                }
            }
        }
        Err(_) => {
            println!("Error occured while deserializing reset tickets message");
        }
    }
    client_info_return 
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
            }
            MessageUnion::TicketSale => {
                message_type = MessageUnion::TicketSale;
            }
            MessageUnion::ScalperInfo => {
                message_type = MessageUnion::ScalperInfo;
            }
            MessageUnion::HelloWorld => {
                message_type = MessageUnion::HelloWorld;
            }
            MessageUnion::ResetTickets => {
                message_type = MessageUnion::ResetTickets;
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
    }

    #[test]
    fn test_verify_flat_buffers() {
        let ip_address = "192.168.0.1".to_string();
        let port = 12345;
        let buffer = serialize_client_info(&ip_address, port);

        let (root_buffer, message_type) = verify_root_table(&buffer);
        assert_eq!(message_type, MessageUnion::ClientInfo);
    }

    #[test]
    fn test_hello_world_serialize_and_deserialize() {
        let buffer = serialize_hello_world();
        let (root_buffer, message_type) = verify_root_table(&buffer);
        assert_eq!(message_type, MessageUnion::HelloWorld);
        let msg = deserialize_hello_world(root_buffer);
        match msg {
            Some(hello) => {
                assert_eq!(hello.message(), Some("Hello world!"));
            }
            None => {
                assert_eq!(1, 2, "Failed to deserialize hello world!");
            }
        }
    }

    #[test]
    fn test_scalper_info_serialize_and_deserialize() {
        let ip_addresses: Vec<String> = [
            "192.168.0.1:5555".to_string(),
            "192.168.0.2:5556".to_string(),
        ]
        .to_vec();
        let buffer = serialize_scalper_info(&ip_addresses);
        let (root_buffer, message_type) = verify_root_table(&buffer);
        assert_eq!(message_type, MessageUnion::ScalperInfo);
        let msg = deserialize_scalper_info(root_buffer);
        match msg {
            Some(scalper_info) => match scalper_info.scalper_address() {
                Some(scalper_addresses) => {
                    assert_eq!(scalper_addresses.len(), ip_addresses.len());
                    for (index, item) in ip_addresses.iter().enumerate() {
                        assert_eq!(item, scalper_addresses.get(index));
                    }
                }
                None => {
                    assert_eq!(1, 2, "Failed to deserialize scalper info!");
                }
            },
            None => {
                assert_eq!(1, 2, "Failed to deserialize scalper info!");
            }
        }
    }

    #[test]
    fn test_ticket_sale_serialize_and_deserialize() {
        let money = 20;
        let ticket_number = 30;
        let buffer = serialize_ticket_sale(money, ticket_number);
        let (root_buffer, message_type) = verify_root_table(&buffer);
        assert_eq!(message_type, MessageUnion::TicketSale);
        let msg = deserialize_ticket_sale(root_buffer);
        match msg {
            Some(ticket_sale) => {
                assert_eq!(ticket_sale.money(), money);
                assert_eq!(ticket_sale.ticket_number(), ticket_number);
            }
            None => {
                assert_eq!(1, 2, "Failed to deserialize scalper info!");
            }
        }
    }

    #[test]
    fn test_reset_ticket_serialize_and_deserialize() {
        let reset_event_flag = true;
        let buffer = serialize_reset_tickets(reset_event_flag);
        let (root_buffer, message_type) = verify_root_table(&buffer);
        assert_eq!(message_type, MessageUnion::ResetTickets);

        let msg = deserialize_reset_tickets(root_buffer);
        match msg{
            Some(reset_tickets) => {
                assert_eq!(reset_tickets.reset_event(), reset_event_flag);
            }
            None => {
                assert_eq!(1, 2, "Failed to deserialize reset ticktes message!");
            }
        }
    }

    #[test]
    fn test_client_info_serialize_and_deserialize() {
        let ip_address = "192.168.0.1".to_string();
        let port = 12345;
        let buffer = serialize_client_info(&ip_address, port);
        let (root_buffer, message_type) = verify_root_table(&buffer);
        assert_eq!(message_type, MessageUnion::ClientInfo);

        let msg = deserialize_client_info(root_buffer);
        match msg{
            Some(client_info) => {
                assert_eq!(client_info.port(), port);
            }
            None => {
                assert_eq!(1, 2, "Failed to deserialize reset ticktes message!");
            }
        }

    }
}
