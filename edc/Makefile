EDJECC		=	edje_cc

EDC_FILES	=	info_screen main_menu main_menu_bar option_menu sub_menu download download_perc
EDJ		=	$(addsuffix .edj, $(EDC_FILES))

all: $(EDJ)

$(EDJ): %.edj: %.edc
	@echo " EDJE_CC " $@
	@$(EDJECC) $<
	@[ -d ../release/theme ] || mkdir -p ../release/theme
	@cp $@ ../release/theme/$@

clean:
	@echo "EDJ clean.."
	@rm -rf *.edj

.PHONY: all clean
