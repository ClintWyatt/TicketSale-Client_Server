
pub struct Client {
    money: u32,
    ip_address: String,
    port: u16,
}

impl Client {

    fn new(input_money: u32, input_ip_address: &str, input_port: u16) -> Self{
        Self {money: input_money, ip_address: input_ip_address.to_string(), port: input_port}
    }
}