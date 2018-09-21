/* $Id$ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

 /** @file tbtr_template_gui_main.h Main window for template configuration and overview. */

#ifndef TBTR_GUI_H
#define TBTR_GUI_H

#include "stdafx.h"
#include "window_gui.h"
#include "vehicle_gui_base.h"

class TbtrGui : public Window {
public:
    TbtrGui(WindowDesc*);
    virtual void UpdateWidgetSize(int, Dimension*, const Dimension&, Dimension*, Dimension*);
};

void ShowTbtrGui();

#endif /* !TBTR_GUI_H */
