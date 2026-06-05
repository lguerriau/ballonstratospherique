#include "LoRa.h"

void tx_f(txData *tx){
    printf("tx done \n");
}

int main(){

    char txbuf[255];
    LoRa_ctl modem;

    //See for typedefs, enumerations and there values in LoRa.h header file
    modem.spiCS = 0;//Raspberry SPI CE pin number
    modem.tx.callback = tx_f;
    modem.tx.data.buf = txbuf;
    modem.tx.data.buf[0]='<';
    modem.tx.data.buf[1]=0xff;
    modem.tx.data.buf[2]=0x01;
    memcpy(modem.tx.data.buf+3, "F4KMN-8>APLT00,WIDE1-1:!4800.48N/00011.98E_/A=000268", 53);//copy data we'll sent to buffer
    modem.tx.data.size = 53+3;//Payload len
    modem.eth.preambleLen=6;
    modem.eth.bw = BW125;//Bandwidth 125KHz
    modem.eth.sf = SF12;//Spreading Factor 12
    modem.eth.ecr = CR5;//Error coding rate CR5/8
    modem.eth.CRC = 1;//Turn on CRC checking
    modem.eth.freq = 433775000;// 434.8MHz
    modem.eth.resetGpioN = 0;//GPIO4 on lora RESET pin
    modem.eth.dio0GpioN = 17;//GPIO17 on lora DIO0 pin to control Rxdone and Txdone interrupts
    modem.eth.outPower = OP20;//Output power
    modem.eth.powerOutPin = PA_BOOST;//Power Amplifire pin
    modem.eth.AGC = 1;//Auto Gain Control
    modem.eth.OCP = 240;// 45 to 240 mA. 0 to turn off protection
    modem.eth.implicitHeader = 0;//Explicit header mode
    modem.eth.syncWord = 0x12;
    //For detail information about SF, Error Coding Rate, Explicit header, Bandwidth, AGC, Over current protection and other features refer to sx127x datasheet https://www.semtech.com/uploads/documents/DS_SX1276-7-8-9_W_APP_V5.pdf

    LoRa_begin(&modem);
    LoRa_send(&modem);

    printf("Tsym: %f\n", modem.tx.data.Tsym);
    printf("Tpkt: %f\n", modem.tx.data.Tpkt);
    printf("payloadSymbNb: %u\n", modem.tx.data.payloadSymbNb);

    printf("sleep %d seconds to transmitt complete\n", (int)modem.tx.data.Tpkt/1000);
    sleep(((int)modem.tx.data.Tpkt/1000)+1);

    printf("end\n");

    LoRa_end(&modem);
}
