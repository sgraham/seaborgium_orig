#include "skin.h"

// Temp manual config; Solarized Dark
Gwen::Color kBase03(0, 43, 54);
Gwen::Color kBase02(7, 54, 66);
Gwen::Color kBase01(88, 110, 117);
Gwen::Color kBase00(101, 123, 131);
Gwen::Color kBase0(131, 148, 150);
Gwen::Color kBase1(147, 161, 161);
Gwen::Color kBase2(238, 232, 213);
Gwen::Color kBase3(253, 246, 227);
Gwen::Color kYellow(181, 137, 0);
Gwen::Color kOrange(203, 75, 22);
Gwen::Color kRed(220, 50, 47);
Gwen::Color kMagenta(211, 54, 130);
Gwen::Color kViolet(108, 113, 196);
Gwen::Color kBlue(38, 139, 210);
Gwen::Color kCyan(42, 161, 152);
Gwen::Color kGreen(133, 153, 0);

ColorScheme::ColorScheme() :
    border_(0, 0, 0),
    background_(kBase03),
    text_(kBase0),
    title_bar_active_(kBase02),
    title_bar_text_active_(kCyan),
    title_bar_inactive_(kBase03),
    title_bar_text_inactive_(kBase02) {
}
