#include "ibm.h"
#include "mouse.h"
#include "serial.h"

static int oldb=0;

void mouse_serial_poll(int x, int y, int b)
{
        uint8_t mousedat[3];
        
        if (!(serial.ier&1)) return;
        if (!x && !y && b==oldb) return;

        oldb=b;
        if (x>127) x=127;
        if (y>127) y=127;
        if (x<-128) x=-128;
        if (y<-128) y=-128;

        /*Use Microsoft format*/
        mousedat[0]=0x40;
        mousedat[0]|=(((y>>6)&3)<<2);
        mousedat[0]|=((x>>6)&3);
        if (b&1) mousedat[0]|=0x20;
        if (b&2) mousedat[0]|=0x10;
        mousedat[1]=x&0x3F;
        mousedat[2]=y&0x3F;
        
        if (!(serial.mctrl&0x10))
        {
                serial_write_fifo(mousedat[0]);
                serial_write_fifo(mousedat[1]);
                serial_write_fifo(mousedat[2]);
        }
}

void mouse_serial_rcr()
{
        mousepos=-1;
        mousedelay=1000;
}
        
void mousecallback()
{
        if (mousepos == -1)
        {
                mousepos = 0;
                serial_fifo_read = serial_fifo_write = 0;
                serial.linestat &= ~1;
                serial_write_fifo('M');
        }
        else if (serial_fifo_read != serial_fifo_write)
        {
                serial.iir=4;
                serial.linestat|=1;
                if (serial.mctrl&8) picint(0x10);
        }
//        printf("Mouse callback\n");
}

void mouse_serial_init()
{
        mouse_poll = mouse_serial_poll;
        serial_rcr = mouse_serial_rcr;
}

