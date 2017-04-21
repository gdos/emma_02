#ifndef SPLASH_H
#define SPLASH_H

class SplashScreen : public wxDialog
{
public:
    SplashScreen(wxWindow *parent);
    ~SplashScreen();

private:
    void noShow(wxCommandEvent&event);
    void onClose(wxCloseEvent&event);
    void onTimer(wxTimerEvent&event);
      
    wxTimer *timerPointer;
    
    bool vt100_;
    
    DECLARE_EVENT_TABLE()
};

#endif  // SPLASH_H
