EDJECC		=	edje_cc

EDC_FILES	=	info_screen main_menu main_menu_bar option_menu sub_menu download download_perc error
EDJ		=	$(addsuffix .edj, $(EDC_FILES))

all: $(EDJ)

$(EDJ): %.edj: %.edc
	@echo " EDJE_CC " $@
	@$(EDJECC) $<
	@[ -d ../release/USRDIR ] || mkdir -p ../release/USRDIR
	@[ -d ../release/USRDIR/theme ] || mkdir -p ../release/USRDIR/theme
	@cp $@ ../release/USRDIR/theme/$@

clean:
	@echo "EDJ clean.."
	@rm -rf *.edj *~

.PHONY: all clean
