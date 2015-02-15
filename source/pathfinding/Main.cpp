#include <stdio.h>
#include <stdlib.h>

#include <snp/snp.h>
using namespace snp;

int main(int argc, char* argv[])
{
	{
		// test initialization
		tmDevice<32> oDevice1;
		// must be succeded
		CErrorCode result1 = oDevice1.Configure(32, 32);

		tmDevice<64> oDevice2;
		// must be failed despite of different bitwidth (GPU is busy)
		CErrorCode result2 = oDevice2.Configure(32, 32);
		
		// as it wasn't configured the releasing must be failed
		CErrorCode result3 = oDevice2.End();

		// must be succeeded
		CErrorCode result4 = oDevice1.End();

		// initialization must be okay now
		CErrorCode result5 = oDevice2.Configure(32, 32);
	}

	typedef tmDevice<128> tDevice;

	tDevice oDevice;
	// must be configured successfully as previous device was
	// released when we quitted his scope
	CErrorCode oResult = oDevice.Configure(4, 4);
	tDevice::tInstruction oInstruction;

	// address all cells at once (mask doesn't cover any of bits)
    snpBitfieldSet(oInstruction.m_oSearchMask._raw, 0);
	snpBitfieldSet(oInstruction.m_oSearchTag._raw, 0);
	// write constant values to the cells
	snpBitfieldSet(oInstruction.m_oWriteMask._raw, ~0);
	oInstruction.m_oWriteData._raw[0] = 1;
	oInstruction.m_oWriteData._raw[1] = 2;
	oInstruction.m_oWriteData._raw[2] = 3;
	oInstruction.m_oWriteData._raw[3] = 4;
	// perform instruction
	oDevice.Exec(false, tOperation_Assign, oInstruction);

	// for now all cells must have the same value
	// let read them all using the first cell as flag
	oInstruction.m_oSearchMask._raw[0] = ~0;
	oInstruction.m_oSearchTag._raw[0] = 1;
	// after cell is address just reset this flag
	snpBitfieldSet(oInstruction.m_oWriteMask._raw, 0);
	oInstruction.m_oWriteMask._raw[0] = ~0;
	oInstruction.m_oWriteData._raw[0] = 0;

	tDevice::tBitfield oBitfield;
	while(oDevice.Exec(true, tOperation_Assign, oInstruction) == true)
	{
		// result must be true in any case
		bool bResult = oDevice.Read(oBitfield);
        if (!bResult)
            printf("Error while reading!");
	}

	return 0;
}
