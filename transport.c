#include"chaos.h"

union Payload p;
char result;

void LoadInitPayload() {
}

int main()
{
	Chaos_slaveInit(9600);
	Choas_slaveSetOperation(SlaveInit);
	Chaos_Read(&p);
	Choas_slaveSaveAddress(p.content.srcAddress);
	Choas_slaveSetOperation(SlaveNOP);
	Chaos_masterInit(9600);
	Choas_RFinit(9600);
	while (1) {
		Chaos_masterGetOperation()(&p);
		Chaos_RFscan(&p);
	}
}