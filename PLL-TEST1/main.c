#include    "Solar_F.h"
// #include    "math.h" // 2/6 chieu nay test thu !!! // van chua fix duoc PLL 
#include    "Setup_PLL.h"
#include    "F28x_Project.h"
extern float sin_tab[];

#define GRID_FREQ 50
#define ISR_FREQUENCY 20000
#define PI 3.14159265359
SPLL_1ph_SOGI_F spll1;
double  tam;
int count = 0 ;
float  goc, sin_in;
float index = 0 ;
long int dem =0;
int  AdcaResult0 =0 ;
 void main(void)
{
       InitSysCtrl();
       GPIOsetup();
       ConfigureDAC();
       SetupADCSoftware();
       DINT;
       InitPieCtrl();
       IER = 0x0000;
       IFR = 0x0000;
       InitPieVectTable();
       EALLOW;
       PieVectTable.TIMER0_INT = &cpu_timer0_isr;
       PieVectTable.TIMER1_INT = &cpu_timer1_isr;
       PieVectTable.TIMER2_INT = &cpu_timer2_isr;
       EDIS;
       InitCpuTimers();
       ConfigCpuTimer(&CpuTimer0, 200,50);
       ConfigCpuTimer(&CpuTimer1, 200, 100);
       ConfigCpuTimer(&CpuTimer2, 200, 100);
       CpuTimer0Regs.TCR.all = 0x4000;
       CpuTimer1Regs.TCR.all = 0x4000;
       CpuTimer2Regs.TCR.all = 0x4000;
          IER |= M_INT1;
          IER |= M_INT13;
          IER |= M_INT14;
          PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
          EINT;
          ERTM;
          ConfigureADC();
    SPLL_1ph_SOGI_F_init(GRID_FREQ,((float)(1.0/ISR_FREQUENCY)),&spll1);
    SPLL_1ph_SOGI_F_coeff_update(((float)(1.0/ISR_FREQUENCY)),(float)(2*PI*GRID_FREQ),&spll1);
    while(1)
    {
        AdcaRegs.ADCSOCFRC1.all = 0x0003; //SOC0 and SOC1
        while(AdcaRegs.ADCINTFLG.bit.ADCINT1 == 0);
        AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
        AdcaResult0 = AdcaResultRegs.ADCRESULT0;
// ADC input range2 > 3 ~ 0->4096, Can setup rms dien ap de cam bien ->2-3V => 3--> 4096
        sin_in =  (AdcaResult0) * 4096/3.3;
        sin_in = tam-3104;
         sin_in=tam / 621;
         DacbRegs.DACVALS.all =sin_in * 500+1000  ;// output PLL
    }
}
void GPIOsetup(void)
{
 EALLOW;
      GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;// I/O
      GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;// I/O
      GpioCtrlRegs.GPBDIR.bit.GPIO34  = 1;// Output
      GpioCtrlRegs.GPADIR.bit.GPIO31  = 1;// Output
  EDIS;
}
__interrupt void cpu_timer0_isr(void)
   {
      CpuTimer0.InterruptCount++;
//      index = index + 5.12;
//         if(index >= 2048)
//             index = 0;
//         sin_in = sin_tab[(int)index];

         spll1.u[0]=(float32) sin_in;
         SPLL_1ph_SOGI_F_FUNC(&spll1);
      PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
//      MPPT_PNO_F mppt_pno1;
   }

__interrupt void cpu_timer2_isr(void)
   {
      CpuTimer2.InterruptCount++;
      count++;
      if (count <=5000)
      {
          GpioDataRegs.GPBDAT.bit.GPIO34 = 0;
          GpioDataRegs.GPADAT.bit.GPIO31 = 1;
      }
      else
      {
          GpioDataRegs.GPBDAT.bit.GPIO34 = 1;
          GpioDataRegs.GPADAT.bit.GPIO31 = 0;
          if (count >=10000)
              count = 0;
      }

//      if (goc >2*PI)
//        goc = 0;
//    //??????????????????????????????????
//          index = (int) (goc*2047/ (2*PI)) ;
//


      PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
   }
__interrupt void cpu_timer1_isr(void)
{

    CpuTimer1.InterruptCount++;



    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
void ConfigureDAC(void) 
{
    EALLOW;
    DacbRegs.DACCTL.bit.DACREFSEL = 1;          // Use ADC references
    DacbRegs.DACCTL.bit.LOADMODE = 0;           // Load on next SYSCLK
                    // Set mid-range
    DacbRegs.DACOUTEN.bit.DACOUTEN = 1;         // Enable DAC

//    DacaRegs.DACCTL.bit.DACREFSEL = 1;          // Use ADC references
//    DacaRegs.DACCTL.bit.LOADMODE = 0;           // Load on next SYSCLK
                    // Set mid-range
    DacaRegs.DACOUTEN.bit.DACOUTEN = 1;         // Enable DAC

    DaccRegs.DACCTL.bit.DACREFSEL = 1;          // Use ADC references
    DaccRegs.DACCTL.bit.LOADMODE = 0;           // Load on next SYSCLK
                        // Set mid-range
    DaccRegs.DACOUTEN.bit.DACOUTEN = 1;         // Enable DAC
    EDIS;
}

void ConfigureADC(void) // doc tin hieu analog tu kenh ADCA
{
    EALLOW;
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    DELAY_US(1000); // 1ms
    EDIS;
}
