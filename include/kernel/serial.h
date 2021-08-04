#pragma  once

int init_serial();

int serial_recieved();
char read_serial();

int is_transmit_empty();
void write_serial(char a);


