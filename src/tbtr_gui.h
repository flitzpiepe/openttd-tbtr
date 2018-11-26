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
#include "tilehighlight_func.h"

#include "tbtr_template_vehicle.h"

typedef GUIList<const Group*> GUIGroupList;
typedef GUIList<const TemplateVehicle*> GUITemplateList;

/*
 * TBTR's main window - for managing templates and setting up train groups for replacement.
 */
class TbtrGui : public Window {
public:
	TbtrGui(WindowDesc*, uint16);
	virtual void UpdateWidgetSize(int, Dimension*, const Dimension&, Dimension*, Dimension*);
	virtual void DrawWidget(const Rect&, int) const;
    virtual void OnClick(Point, int, int);
	virtual void OnPaint();
    virtual void OnResize();
    virtual bool OnVehicleSelect(const Vehicle*);

private:
	void BuildGroupList(Owner);
	void BuildTemplateList(Owner);
	void DrawGroups(const Rect&) const;
	void DrawTemplates(const Rect&) const;
	int FindTemplateInGuiList(TemplateID) const;

	Scrollbar* vscroll_groups;          ///< Scrollbar for the group list
	Scrollbar* vscroll_templates;       ///< Scrollbar for the template list
	uint16 line_height = 26;            ///< step size for the matrix widgets
	int index_selected_group = -1;
	int index_selected_template = -1;
	GUIGroupList groups;                ///< List of groups
    GUITemplateList templates;
    RailType railtype;
};

void ShowTbtrGui(uint16);

#endif /* !TBTR_GUI_H */
