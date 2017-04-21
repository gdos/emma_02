#ifndef GUIMCDS_H
#define GUIMCDS_H

#include "guicosmicos.h"
#include "elfconfiguration.h"

class GuiMcds: public GuiCosmicos
{
public:

	GuiMcds(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir);
	~GuiMcds() {};

	void readMcdsConfig();
    void writeMcdsDirConfig();
	void writeMcdsConfig();
    void readMcdsWindowConfig();
    void writeMcdsWindowConfig();

	void onMcdsBaudT(wxCommandEvent& event);
	void onMcdsClock(wxCommandEvent& event);
    void onMcdsForceUpperCase(wxCommandEvent& event);
    void onBootRam(wxCommandEvent&event);
    
    void setBaudChoiceMcds();

protected:

private:
    wxPoint position_;
    wxString floppyDirSwitchedMcds_[4];
    wxString floppyDirMcds_[4];
    wxString floppyMcds_[4];
    
    bool directoryMode_[4];
    
	DECLARE_EVENT_TABLE()
};

#endif // GUIMCDS_H
