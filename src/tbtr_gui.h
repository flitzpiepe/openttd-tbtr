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
#include "strings_func.h"
#include "window_gui.h"
#include "vehicle_gui_base.h"

#include "group.h"
#include "company_func.h"

typedef GUIList<const Group*> GUIGroupList;

class TbtrGui : public Window {
public:
	TbtrGui(WindowDesc*);
	virtual void UpdateWidgetSize(int, Dimension*, const Dimension&, Dimension*, Dimension*);
	virtual void OnPaint();
	virtual void DrawWidget(const Rect&, int) const;

private:
	void BuildGroupList(Owner);
	void DrawGroups(int, const Rect&) const;

	Scrollbar* vscroll[2];          ///< Scrollbars for the matrix widgets
	int line_height = 12;                ///< step size for the matrix widgets
	int selected_group_index = -1;
	GUIGroupList groups;            ///< List of groups
};

void ShowTbtrGui();

#endif /* !TBTR_GUI_H */
