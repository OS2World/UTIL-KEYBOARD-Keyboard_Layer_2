DFLAGS = -nocdsm -db kl_docs.db

DOCCLS = ../kl_messages.h        \
         ../kl_broker.h          \
         ../kl_pipe.h            \
         ../kl_popup.h           \
         ../kl_systray.h         \
         ../kl_hotkey.h          \
         ../kl_bundle.h          \
         ../kl_viewer.h          \
         ../kl_trigger.h         \
         ../kl_application.h

DOCSET = ../kl_setup.h           \
         ../kl_setuppage.h       \
         ../kl_setuplayer_p1.h   \
         ../kl_setuplayer_p2.h   \
         ../kl_setuplayouts_p1.h \
         ../kl_setuplayouts_p2.h \
         ../kl_setuplanguage.h   \
         ../kl_setupabout.h      \
         ../kl_setuphkeys.h

DOCENG = ../kl_engine.h          \
         ../kl_profiles.h

DOCVDD = ../../VDD/kl_vdd.h

all:
  -@erase *.html
  -@erase *.db
   @ccdoc $(DFLAGS) $(DOCCLS)
   @ccdoc $(DFLAGS) $(DOCENG) -pkg "Engine"
   @ccdoc $(DFLAGS) $(DOCSET) -pkg "Setup"
   @ccdoc $(DFLAGS) $(DOCVDD) -pkg "VDD"
   @ccdoc -db kl_docs.db -index
   @echo  Generating documentation...
   @ccdoc -db kl_docs.db -root KLayer -norptsci -rptdefa "Dmitry A.Steklenev" -norptdpv -rptmac1 -rptpro -bg \"#FFFFFF\" -fg \"#000000\" -html ./ -srcurl ./

clean:
  -@erase *.db
  -@erase *.html
