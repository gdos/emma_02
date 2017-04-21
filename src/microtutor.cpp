/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#if !wxUSE_COMBOCTRL
    #error "Please set wxUSE_COMBOCTRL to 1 and rebuild the library."
#endif

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMGL__)
#include "app_icon.xpm"
#endif

#include "main.h"
#include "pushbutton.h"

#include "microtutor.h"

MicrotutorScreen::MicrotutorScreen(wxWindow *parent, const wxSize& size)
: Panel(parent, size)
{
}

MicrotutorScreen::~MicrotutorScreen()
{
	delete mainBitmapPointer;

    delete runSwitchButton;
    delete loadSwitchButton;
    delete inSwitchButton;
    delete clearSwitchButton;
    
    for (int i=0; i<8; i++)
    {
        delete dataSwitchButton[i];
    }
    for (int i=0; i<2; i++)
	{
		delete dataPointer[i];
	}
}

void MicrotutorScreen::init()
{
	keyStart_ = 0;
	keyEnd_ = 0;
	lastKey_ = 0;

	wxClientDC dc(this);

	runSwitchButton = new SwitchButton(dc, PUSH_BUTTON_BLACK, wxColour(43, 71, 106), BUTTON_UP, 284, 30, "");
    loadSwitchButton = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(43, 71, 106), BUTTON_DOWN, 62, 30, "");
    inSwitchButton = new SwitchButton(dc, PUSH_BUTTON_BLACK, wxColour(43, 71, 106), BUTTON_UP, 25, 30, "");
    clearSwitchButton = new SwitchButton(dc, PUSH_BUTTON, wxColour(43, 71, 106), BUTTON_UP, 247, 30, "");
    
    for (int i=0; i<8; i++)
    {
        if (i==0 || i==4)
            dataSwitchButton[i] = new SwitchButton(dc, VERTICAL_BUTTON_RED, wxColour(43, 71, 106), BUTTON_DOWN, 25+37*(7-i), 100, "");
        else
            dataSwitchButton[i] = new SwitchButton(dc, VERTICAL_BUTTON, wxColour(43, 71, 106), BUTTON_DOWN, 25+37*(7-i), 100, "");
    }
    
	for (int i=0; i<2; i++)
	{
		dataPointer[i] = new Til311();
		dataPointer[i]->init(dc, 145+i*24, 30);
	}

	mainBitmapPointer = new wxBitmap(p_Main->getApplicationDir() + "images/microtutor.png", wxBITMAP_TYPE_PNG);

	this->connectKeyEvent(this);
}

void MicrotutorScreen::onPaint(wxPaintEvent&WXUNUSED(event))
{
	wxPaintDC dc(this);
	dc.DrawBitmap(*mainBitmapPointer, 0, 0);

	for (int i=0; i<2; i++)
	{
		dataPointer[i]->onPaint(dc);
	}
    
    runSwitchButton->onPaint(dc);
    loadSwitchButton->onPaint(dc);
    inSwitchButton->onPaint(dc);
    clearSwitchButton->onPaint(dc);

    for (int i=0; i<8; i++)
    {
        dataSwitchButton[i]->onPaint(dc);
    }
}

void MicrotutorScreen::onMouseRelease(wxMouseEvent&event)
{
	int x, y;
	event.GetPosition(&x, &y);

	wxClientDC dc(this);

    if (runSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onRunButtonRelease();
    if (loadSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onLoadButton();
    if (inSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onInButtonRelease();
    if (clearSwitchButton->onMouseRelease(dc, x, y))
        p_Computer->onClearButtonRelease();

    for (int i=0; i<8; i++)
    {
        if (dataSwitchButton[i]->onMouseRelease(dc, x, y))
            p_Computer->dataSwitch(i);
    }
}

void MicrotutorScreen::onMousePress(wxMouseEvent&event)
{
    int x, y;
    event.GetPosition(&x, &y);
    
    wxClientDC dc(this);
    
    if (runSwitchButton->onMousePress(dc, x, y))
        p_Computer->onRunButtonPress();
    if (inSwitchButton->onMousePress(dc, x, y))
        p_Computer->onInButtonPress(p_Computer->getData());
    if (clearSwitchButton->onMousePress(dc, x, y))
        p_Computer->onClearButtonPress();
}

BEGIN_EVENT_TABLE(Microtutor, wxFrame)
	EVT_CLOSE (Microtutor::onClose)
END_EVENT_TABLE()

Microtutor::Microtutor(const wxString& title, const wxPoint& pos, const wxSize& size, double clock, ElfConfiguration conf)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	microtutorConfiguration = conf;
	microtutorClockSpeed_ = clock;
	data_ = 0;

#ifndef __WXMAC__
	SetIcon(wxICON(app_icon));
#endif

	this->SetClientSize(size);

	microtutorScreenPointer = new MicrotutorScreen(this, size);
	microtutorScreenPointer->init();
}

Microtutor::~Microtutor()
{
	p_Main->setMainPos(MICROTUTOR, GetPosition());

	delete microtutorScreenPointer;
}

void Microtutor::onClose(wxCloseEvent&WXUNUSED(event) )
{
	p_Main->stopComputer();
}

bool Microtutor::keyUpReleased(int key)
{
	if (key == inKey1_ || key == inKey2_)
	{
		onInButtonRelease();
		return true;
	}
	return false;
}

void Microtutor::onRunButtonPress()
{
	onRun();
    microtutorScreenPointer->runSetState(BUTTON_DOWN);
}

void Microtutor::onRunButtonRelease()
{
    microtutorScreenPointer->runSetState(BUTTON_UP);
}

void Microtutor::onRun() 
{
    showData(dmaOut());
    if (getCpuMode() != LOAD)
    {
        setClear(1);
		setWait(1);
		p_Main->eventUpdateTitle();
		p_Main->startTime();
	}
}

void Microtutor::autoBoot()
{
	setClear(1);
}

Byte Microtutor::inPressed()
{
    if (inPressed_ == true)
        return 0;
    else
        return 1;
}

void Microtutor::onInButtonPress(Byte value)
{
    if (loadButtonState_ == 1)
        inPressed_ = true;
    if (getCpuMode() == LOAD)
    {
        dmaIn(value);
        showData(value);
    }
    microtutorScreenPointer->inSetState(BUTTON_DOWN);
}

void Microtutor::onInButtonRelease()
{
    if (loadButtonState_ == 1)
        inPressed_ = false;
    microtutorScreenPointer->inSetState(BUTTON_UP);
}

void Microtutor::onLoadButton()
{
	if (!loadButtonState_)
	{
		loadButtonState_ = 1;
	}
	else
	{
		loadButtonState_ = 0;
	}
	setWait(loadButtonState_);
}

void Microtutor::onClearButtonPress()
{
    //   for (int i=0; i<8; i++)
    //   {
    //       dataSwitchState_[i] = 0;
    //   }
	setClear(0);
    resetCpu();

    microtutorScreenPointer->clearSetState(BUTTON_DOWN);
}

void Microtutor::onClearButtonRelease()
{
    microtutorScreenPointer->clearSetState(BUTTON_UP);
}

void Microtutor::dataSwitch(int i)
{
	if (dataSwitchState_[i])
	{
		dataSwitchState_[i] = 0;
	}
	else
	{
		dataSwitchState_[i] = 1;
	}
}

Byte Microtutor::getData()
{
	return(dataSwitchState_[7]?128:0) +(dataSwitchState_[6]?64:0) +(dataSwitchState_[5]?32:0) +(dataSwitchState_[4]?16:0) +(dataSwitchState_[3]?8:0) +(dataSwitchState_[2]?4:0) +(dataSwitchState_[1]?2:0) +(dataSwitchState_[0]?1:0);
}

void Microtutor::configureComputer()
{
	inType_[0] = MICROTUTORIN;
    outType_[0] = MICROTUTOROUT;
    efType_[4] = MICROTUTOREF;
    
	p_Main->message("Configuring Microtutor");
	p_Main->message("	Output 0: display output, input 0: data input");
    p_Main->message("	EF 4: 0 when in button pressed");
    p_Main->message("");

	inKey1_ = p_Main->getDefaultInKey1("Membership");
	inKey2_ = p_Main->getDefaultInKey2("Membership");

	resetCpu();
}

void Microtutor::initComputer()
{
	Show(true);

	loadButtonState_ = 1;

	for (int i=0; i<8; i++)  dataSwitchState_[i]=0;
    
    switches_ = 0;
    inPressed_ = false;
}

Byte Microtutor::ef(int flag)
{
	switch(efType_[flag])
	{
		case 0:
			return 1;
		break;

		case MICROTUTOREF:
			return inPressed();
		break;

		default:
			return 1;
	}
}

Byte Microtutor::in(Byte port, Word WXUNUSED(address))
{
	Byte ret;
	ret = 0;

	switch(inType_[port])
	{
		case 0:
			ret = 255;
		break;

		case MICROTUTORIN:
            inPressed_ = false;
            ret = getData();
		break;

		default:
			ret = 255;
	}
	inValues_[port] = ret;
	return ret;
}

void Microtutor::out(Byte port, Word WXUNUSED(address), Byte value)
{
	outValues_[port] = value;

	switch(outType_[port])
	{
		case 0:
			return;
		break;

		case MICROTUTOROUT:
            inPressed_ = false;
            showData(value);
		break;
	}
}

void Microtutor::showData(Byte val)
{
	microtutorScreenPointer->showData(val);
}

void Microtutor::cycle(int type)
{
	switch(cycleType_[type])
	{
		case 0:
			return;
		break;
	}
}

void Microtutor::startComputer()
{
	resetPressed_ = false;

    defineMemoryType(0, 0xff, RAM);
    defineMemoryType(0x100, 0xffff, MAPPEDRAM);
    initRam(0, 0xff);

	p_Main->assDefault("mycode", 0, 0xA0);

	readProgram(p_Main->getRomDir(MICROTUTOR, MAINROM1), p_Main->getRomFile(MICROTUTOR, MAINROM1), RAM, 0, NONAME);

	if (microtutorConfiguration.autoBoot)
	{
        scratchpadRegister_[0]=p_Main->getBootAddress("Microtutor", MICROTUTOR);
        autoBoot();
	}

	p_Main->setSwName("");
	p_Main->updateTitle();

	cpuCycles_ = 0;
	p_Main->startTime();
	microtutorScreenPointer->setLedMs(p_Main->getLedTimeMs(MICROTUTOR));

	threadPointer->Run();
}

void Microtutor::writeMemDataType(Word address, Byte type)
{
    if (mainMemoryDataType_[address] != type)
    {
        p_Main->updateAssTabCheck(scratchpadRegister_[programCounter_]);
        mainMemoryDataType_[address] = type;
    }
}

Byte Microtutor::readMemDataType(Word address)
{
    return mainMemoryDataType_[address];
}

Byte Microtutor::readMem(Word addr)
{
	address_ = addr;

	switch (memoryType_[addr / 256])
	{
		case UNDEFINED:
			return 255;
		break;

		case RAM:
		case MAPPEDRAM:
			return mainMemory_[addr];
		break;

		default:
			return 255;
		break;
	}
}

void Microtutor::writeMem(Word addr, Byte value, bool writeRom)
{
	address_ = addr;

	switch (memoryType_[addr/256])
	{
		case UNDEFINED:
			if (writeRom)
				mainMemory_[addr]=value;
		break;

		case MAPPEDRAM:
				if (mainMemory_[addr]==value)
					return;
				mainMemory_[addr]=value;
				if (address_ >= memoryStart_ && address_<(memoryStart_ +256))
					p_Main->updateDebugMemory(addr);
				p_Main->updateAssTabCheck(address_);
		break;

		case RAM:
			if (mainMemory_[address_]==value)
				return;
			mainMemory_[address_]=value;
			if (address_ >= memoryStart_ && address_<(memoryStart_  +256))
				p_Main->updateDebugMemory(addr);
			p_Main->updateAssTabCheck(address_);
		break;
	}
}

void Microtutor::cpuInstruction()
{
	if (cpuMode_ == RUN)
	{
		if (steps_ != 0)
		{
			cycle0_=0;
			machineCycle();
			if (cycle0_ == 0) machineCycle();
			if (cycle0_ == 0 && steps_ != 0)
			{
				cpuCycle();
				cpuCycles_ += 2;
			}
			if (debugMode_)
				p_Main->showInstructionTrace();
		}
		else
			soundCycle();
		playSaveLoad();
		if (resetPressed_)
		{
			resetCpu();
			initComputer();
			if (microtutorConfiguration.autoBoot)
			{
                scratchpadRegister_[0]=p_Main->getBootAddress("Microtutor", MICROTUTOR);
                autoBoot();
			}

            resetPressed_ = false;
			p_Main->setSwName("");
            p_Main->eventUpdateTitle();
		}
		if (debugMode_)
			p_Main->cycleDebug();
	}
//	else
//	{
//		machineCycle();
//		machineCycle();
//		cpuCycles_ = 0;
//		p_Main->startTime();
//		if (cpuMode_ == LOAD)
//		{
//			showData(readMem(address_));
//			threadPointer->Sleep(1);
//		}
//	}
}

void Microtutor::onReset()
{
	resetPressed_ = true;
}

void Microtutor::ledTimeout()
{
	microtutorScreenPointer->ledTimeout();
}

void Microtutor::setLedMs(long ms)
{
	microtutorScreenPointer->setLedMs(ms);
}

void Microtutor::activateMainWindow()
{
	bool maximize = IsMaximized();
	Iconize(false);
	Raise();
	Show(true);
	Maximize(maximize);
}