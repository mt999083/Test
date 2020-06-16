#include "Solar_F.h"

//*********** Structure Init Function ****//
void MPPT_PNO_F_init(MPPT_PNO_F *v)
{
	v->Ipv=0;
	v->Vpv=0;
	v->DeltaPmin=(0.00001);
	v->MaxVolt=(0.9);
	v->MinVolt=0;
	v->Stepsize=(0.002);
	v->VmppOut=0;
	v->DeltaP=0;
	v->PanelPower=0;
	v->PanelPower_Prev=0;
	v->mppt_enable=1;
	v->mppt_first=1;
}

//*********** Function Definition ********//
void MPPT_PNO_F_FUNC(MPPT_PNO_F *v)
{
	if (v->mppt_enable==1)
		{
			if (v->mppt_first == 1)
			{
				v->VmppOut= v->Vpv - (0.02);
				v->mppt_first=0;
				v->PanelPower_Prev=v->PanelPower;
			}

			// khoi tao ban dau cho lan dau tien mppt
			//=============================================
			else
			{
				v->PanelPower=(v->Vpv*v->Ipv); // P(n) = U *I
				v->DeltaP=v->PanelPower-v->PanelPower_Prev;// delta P = P(n) - P(n-1)
				if (v->DeltaP > v->DeltaPmin)// so sanh voi cs min
				{
					v->VmppOut=v->Vpv+v->Stepsize; // Vout = Vpv + Step
				}
				else
				{
					if (v->DeltaP < -v->DeltaPmin) /// Vout = vpv - step
					{
						v->Stepsize=-v->Stepsize;
						v->VmppOut=v->Vpv+v->Stepsize;
					}
				}
				v->PanelPower_Prev = v->PanelPower;
			}
			if(v->VmppOut < v->MinVolt) v->VmppOut = v->MinVolt;
			if(v->VmppOut > v->MaxVolt) v->VmppOut = v->MaxVolt;
		}
}