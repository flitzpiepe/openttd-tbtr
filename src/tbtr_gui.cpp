/*
 * tbtr_gui.cpp
 * Copyright (C) 2018 streen <streen@gentoobox>
 *
 * Distributed under terms of the MIT license.
 */

#include "stdafx.h"
#include "string_func.h"
#include "tbtr_gui.h"
#include "command_func.h"
#include "engine_gui.h"


enum TemplateReplaceWindowWidgets {
	TRW_CAPTION,

	TRW_WIDGET_INSET_GROUPS,
	TRW_WIDGET_TOP_MATRIX,
	TRW_WIDGET_TOP_SCROLLBAR,

	TRW_WIDGET_INSET_TEMPLATES,
	TRW_WIDGET_BOTTOM_MATRIX,
	TRW_WIDGET_BOTTOM_SCROLLBAR,

	TRW_WIDGET_TMPL_INFO_INSET,
	TRW_WIDGET_TMPL_INFO_PANEL,

	TRW_WIDGET_TMPL_PRE_BUTTON_FLUFF,

	TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_REUSE,
	TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_KEEP,
	TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_REFIT,
	TRW_WIDGET_TMPL_BUTTONS_CONFIG_RIGHTPANEL,

	TRW_WIDGET_TMPL_BUTTONS_DEFINE,
	TRW_WIDGET_TMPL_BUTTONS_EDIT,
	TRW_WIDGET_TMPL_BUTTONS_CLONE,
	TRW_WIDGET_TMPL_BUTTONS_DELETE,
	TRW_WIDGET_TMPL_BUTTONS_RPLALL,
	TRW_WIDGET_TMPL_BUTTON_FLUFF,
	TRW_WIDGET_TMPL_BUTTONS_EDIT_RIGHTPANEL,

	TRW_WIDGET_TITLE_INFO_GROUP,
	TRW_WIDGET_TITLE_INFO_TEMPLATE,

	TRW_WIDGET_INFO_GROUP,
 	TRW_WIDGET_INFO_TEMPLATE,

	TRW_WIDGET_TMPL_BUTTONS_SPACER,

	TRW_WIDGET_START,
	TRW_WIDGET_TRAIN_FLUFF_LEFT,
	TRW_WIDGET_TRAIN_RAILTYPE_DROPDOWN,
	TRW_WIDGET_TRAIN_FLUFF_RIGHT,
	TRW_WIDGET_STOP,

	TRW_WIDGET_SEL_TMPL_DISPLAY_CREATE,

	// new engines
	TRW_WIDGET_NEW_ENGINES_MATRIX,
};

static const NWidgetPart _widgets[] = {
	/* Title bar */
	NWidget(NWID_HORIZONTAL),
		NWidget(WWT_CLOSEBOX, COLOUR_GREY),
		NWidget(WWT_CAPTION, COLOUR_GREY, TRW_CAPTION), SetDataTip(STR_TBTR_RPL_TITLE, STR_TOOLTIP_WINDOW_TITLE_DRAG_THIS),
		NWidget(WWT_SHADEBOX, COLOUR_GREY),
		NWidget(WWT_STICKYBOX, COLOUR_GREY),
	EndContainer(),

	/* Toplevel container */
	NWidget(NWID_HORIZONTAL),

		/* New engines */
		NWidget(NWID_VERTICAL),
			NWidget(WWT_MATRIX, COLOUR_GREY, TRW_WIDGET_NEW_ENGINES_MATRIX), SetMinimalSize(100, 0), SetFill(1, 1), SetDataTip(0x1, STR_REPLACE_HELP_LEFT_ARRAY), SetResize(1, 0),
		EndContainer(),

		/* Template Ctrl */
		NWidget(NWID_VERTICAL),
			/*Top Matrix */
			NWidget(NWID_VERTICAL),
				NWidget(WWT_INSET, COLOUR_GREY, TRW_WIDGET_INSET_GROUPS), SetMinimalSize(216,12), SetDataTip(STR_TBTR_MAINGUI_DEFINEDGROUPS, STR_TBTR_MAINGUI_DEFINEDGROUPS), SetResize(1, 0), EndContainer(),
				NWidget(NWID_HORIZONTAL),
					NWidget(WWT_MATRIX, COLOUR_GREY, TRW_WIDGET_TOP_MATRIX), SetMinimalSize(216, 0), SetFill(1, 1), SetDataTip(0x1, STR_REPLACE_HELP_LEFT_ARRAY), SetResize(1, 0), SetScrollbar(TRW_WIDGET_TOP_SCROLLBAR),
					NWidget(NWID_VSCROLLBAR, COLOUR_GREY, TRW_WIDGET_TOP_SCROLLBAR),
				EndContainer(),
			EndContainer(),
			/* Template Display */
			NWidget(NWID_VERTICAL),
				NWidget(WWT_INSET, COLOUR_GREY, TRW_WIDGET_INSET_TEMPLATES), SetMinimalSize(216,12), SetDataTip(STR_TBTR_AVAILABLE_TEMPLATES, STR_TBTR_AVAILABLE_TEMPLATES), SetResize(1, 0), EndContainer(),
				NWidget(NWID_HORIZONTAL),
					NWidget(WWT_MATRIX, COLOUR_GREY, TRW_WIDGET_BOTTOM_MATRIX), SetMinimalSize(216, 0), SetFill(1, 1), SetDataTip(0x1, STR_REPLACE_HELP_RIGHT_ARRAY), SetResize(1, 1), SetScrollbar(TRW_WIDGET_BOTTOM_SCROLLBAR),
					NWidget(NWID_VSCROLLBAR, COLOUR_GREY, TRW_WIDGET_BOTTOM_SCROLLBAR),
				EndContainer(),
			EndContainer(),
			/* Info Area */
			NWidget(NWID_VERTICAL),
				NWidget(WWT_INSET, COLOUR_GREY, TRW_WIDGET_TMPL_INFO_INSET), SetMinimalSize(216,12), SetResize(1,0), SetDataTip(STR_TBTR_AVAILABLE_TEMPLATES, STR_TBTR_AVAILABLE_TEMPLATES), EndContainer(),
				NWidget(WWT_PANEL, COLOUR_GREY, TRW_WIDGET_TMPL_INFO_PANEL), SetMinimalSize(216,50), SetResize(1,0), EndContainer(),
			EndContainer(),
			/* Control Area */
			NWidget(NWID_VERTICAL),
				/* Spacing */
				NWidget(WWT_INSET, COLOUR_GREY, TRW_WIDGET_TMPL_PRE_BUTTON_FLUFF), SetMinimalSize(139, 12), SetResize(1,0), EndContainer(),
				/* Config buttons */
				NWidget(NWID_HORIZONTAL),
					NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_REUSE), SetMinimalSize(150,12), SetResize(0,0), SetDataTip(STR_TBTR_SET_USEDEPOT, STR_TBTR_SET_USEDEPOT_TIP),
					NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_KEEP), SetMinimalSize(150,12), SetResize(0,0), SetDataTip(STR_TBTR_SET_KEEPREMAINDERS, STR_TBTR_SET_KEEPREMAINDERS_TIP),
					NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_REFIT), SetMinimalSize(150,12), SetResize(0,0), SetDataTip(STR_TBTR_SET_REFIT, STR_TBTR_SET_REFIT_TIP),
					NWidget(WWT_PANEL, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_CONFIG_RIGHTPANEL), SetMinimalSize(12,12), SetResize(1,0), EndContainer(),
				EndContainer(),
				/* Edit buttons */
				NWidget(NWID_HORIZONTAL),
					NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_DEFINE), SetMinimalSize(75,12), SetResize(0,0), SetDataTip(STR_TBTR_DEFINE_TEMPLATE, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
					NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_EDIT), SetMinimalSize(75,12), SetResize(0,0), SetDataTip(STR_TBTR_EDIT_TEMPLATE, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
					NWidget(WWT_TEXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_CLONE), SetMinimalSize(75,12), SetResize(0,0), SetDataTip(STR_TBTR_CREATE_CLONE_VEH, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
					NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_DELETE), SetMinimalSize(75,12), SetResize(0,0), SetDataTip(STR_TBTR_DELETE_TEMPLATE, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
					NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_RPLALL), SetMinimalSize(150,12), SetResize(0,0), SetDataTip(STR_TBTR_RPL_ALL_TMPL, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
					NWidget(WWT_PANEL, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_EDIT_RIGHTPANEL), SetMinimalSize(50,12), SetResize(1,0), EndContainer(),
				EndContainer(),
			EndContainer(),
			/* Start/Stop buttons */
			NWidget(NWID_HORIZONTAL),
				NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_START), SetMinimalSize(150, 12), SetDataTip(STR_TBTR_RPL_START, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
				NWidget(WWT_PANEL, COLOUR_GREY, TRW_WIDGET_TRAIN_FLUFF_LEFT), SetMinimalSize(15, 12), EndContainer(),
				NWidget(WWT_DROPDOWN, COLOUR_GREY, TRW_WIDGET_TRAIN_RAILTYPE_DROPDOWN), SetMinimalSize(150, 12), SetDataTip(0x0, STR_REPLACE_HELP_RAILTYPE), SetResize(1, 0),
				NWidget(WWT_PANEL, COLOUR_GREY, TRW_WIDGET_TRAIN_FLUFF_RIGHT), SetMinimalSize(16, 12), EndContainer(),
				NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_STOP), SetMinimalSize(150, 12), SetDataTip(STR_TBTR_RPL_STOP, STR_REPLACE_REMOVE_WAGON_HELP),
				NWidget(WWT_RESIZEBOX, COLOUR_GREY),
			EndContainer(),
		EndContainer(),	// END Template Ctrl

	EndContainer(), // END Toplevel container
};

static WindowDesc _tbtr_gui_desc(
	WDP_AUTO,
	"TBTR Gui",
	456, 156,
	WC_TBTR_GUI,
	WC_NONE,
	WDF_CONSTRUCTION,
	_widgets, lengthof(_widgets)
);

/** Sort the groups by their name */
static int CDECL GroupNameSorter(const Group * const *a, const Group * const *b)
{
	static char         last_name[2][64] = { "", "" };
	static const Group *last_group[2] = { NULL, NULL };

	if (*a != last_group[0]) {
		last_group[0] = *a;
		SetDParam(0, (*a)->index);
		GetString(last_name[0], STR_GROUP_NAME, lastof(last_name[0]));
	}

	if (*b != last_group[1]) {
		last_group[1] = *b;
		SetDParam(0, (*b)->index);
		GetString(last_name[1], STR_GROUP_NAME, lastof(last_name[1]));
	}
	int r = strnatcmp(last_name[0], last_name[1]); // Sort by name (natural sorting).
	if (r == 0) return (*a)->index - (*b)->index;
	return r;
}

/**
 * Constructor, initialize GUI with a window descriptor
 */
TbtrGui::TbtrGui(WindowDesc* wdesc) : Window(wdesc)
{
	CreateNestedTree(wdesc);
	this->vscroll_groups = GetScrollbar(TRW_WIDGET_TOP_SCROLLBAR);
	this->vscroll_templates = GetScrollbar(TRW_WIDGET_BOTTOM_SCROLLBAR);
	this->vscroll_groups->SetStepSize(line_height / 2);
	this->vscroll_templates->SetStepSize(line_height);
	/* VEH_TRAIN should be 0; we want only 1 instance of this GUI to be present at the same time anyway, so
	 * this should be ok */
	FinishInitNested(VEH_TRAIN);

	this->line_height = this->resize.step_height * 2;

	/* will be used to build the internal group and template lists
	 *
	 * NOTE: has to be set after FinishInitNested(...) because this function will set the owner back to
	 * INVALID_OWNER again */
	this->owner = _local_company;

	this->engines.ForceRebuild();

	this->groups.ForceRebuild();
	this->groups.NeedResort();
	this->BuildGroupList();
	this->groups.Sort(&GroupNameSorter);

	BuildTemplateList();
}

/*
 * Recalculate the size of the window's components
 */
void TbtrGui::UpdateWidgetSize(int widget, Dimension *size, const Dimension &padding, Dimension *fill, Dimension *resize)
{
	switch (widget)
	{
		case TRW_WIDGET_TOP_MATRIX:
			resize->height = GetVehicleListHeight(VEH_TRAIN, FONT_HEIGHT_NORMAL + WD_MATRIX_TOP) / 2;
			size->height = 8 * resize->height;
			break;
		case TRW_WIDGET_BOTTOM_MATRIX:
			resize->height = GetVehicleListHeight(VEH_TRAIN, FONT_HEIGHT_NORMAL + WD_MATRIX_TOP);
			size->height = 4 * resize->height;
			break;
		case TRW_WIDGET_NEW_ENGINES_MATRIX:
			resize->height = GetVehicleListHeight(VEH_TRAIN, FONT_HEIGHT_NORMAL + WD_MATRIX_TOP);
			size->height = 4 * resize->height;
			break;
	}
}

// TODO comment
void TbtrGui::BuildTemplateEngineList()
{
	if (!this->engines.NeedRebuild()) {
		return;
	}
	this->engines.Clear();
	const Engine* e;
	FOR_ALL_ENGINES(e) {
		*(this->engines).Append() = e;
	}
	this->engines.Compact();
	this->engines.RebuildDone();
}

/*
 * Update the list of groups to display for a given owner.
 * @param owner:  the owner of the groups to display, should the current company when the GUI is opened
 */
void TbtrGui::BuildGroupList()
{
	if (!this->groups.NeedRebuild()) {
		return;
	}

	this->groups.Clear();
	const Group *g;
	FOR_ALL_GROUPS(g) {
		if (g->owner == this->owner) {
			*(this->groups).Append() = g;
		}
	}

	this->groups.Compact();
	this->groups.Sort(&GroupNameSorter);
	this->groups.RebuildDone();
	this->index_selected_group = -1;
}

/*
 * Update the list of templates to display for a given owner and rail type.
 *
 * @param owner
 */
void TbtrGui::BuildTemplateList()
{
	this->templates.Clear();
	const TemplateVehicle *tv;

	FOR_ALL_TEMPLATES(tv) {
		if (tv->HasOwner(this->owner) && (tv->IsPrimaryVehicle() || tv->IsFreeWagonChain()) && tv->ContainsRailType(railtype))
			*(this->templates.Append()) = tv;

	}

	this->templates.RebuildDone();
	this->vscroll_templates->SetCount(this->templates.Length());
}

/*
 * Draw a widget of this GUI
 */
void TbtrGui::DrawWidget(const Rect& r, int widget) const
{
	switch(widget) {
		case TRW_WIDGET_TOP_MATRIX: {
			this->DrawGroups(r);
			break;
		}
		case TRW_WIDGET_BOTTOM_MATRIX: {
			this->DrawTemplates(r);
			break;
		}
		case TRW_WIDGET_NEW_ENGINES_MATRIX: {
			this->DrawEngines(r);
			break;
		}
		case TRW_WIDGET_TMPL_INFO_PANEL: {
			DrawTemplateInfo(r);
			break;
		}
	}
}

/*
 * Draw all engines
 */
void TbtrGui::DrawEngines(const Rect& r) const
{
	// TODO
	if ( this->engines.Length() > 0 )
	{
		const Engine* engine = (this->engines)[0];
		DrawVehicleEngine(r.left, r.right, left, r.top, engine->index, GetEnginePalette(engine->index, this->owner), EIT_PURCHASE);
	}
}

/*
 * Draw all train groups
 */
void TbtrGui::DrawGroups(const Rect& r) const
{
	int left = r.left + WD_MATRIX_LEFT;
	int right = r.right - WD_MATRIX_RIGHT;
	int y = r.top;
	int max = min(this->vscroll_groups->GetPosition() + this->vscroll_groups->GetCapacity(), this->groups.Length());

	/* Then treat all groups defined by/for the current company */
	for ( int i=this->vscroll_groups->GetPosition(); i<max; ++i ) {
		const Group* group = (this->groups)[i];
		short group_id = group->index;

		/* Fill the background of the current cell in a darker tone for the currently selected group */
		if ( this->index_selected_group == i ) {
			GfxFillRect(left, y, right, y+(this->line_height)/2, _colour_gradient[COLOUR_GREY][3]);
		}

		/* Draw the group name */
		SetDParam(0, group_id);
		StringID str = STR_GROUP_NAME;
		DrawString(left+30, right, y+2, str, TC_BLACK);

		/* Draw the index of the selected template for this group
		 * Note, that this is the index into the group list, not the template's ID. */
		if (group->template_id >= 0)
		{
			SetDParam(0, FindTemplateInGuiList(group->template_id));
			DrawString ( left, right, y+2, STR_TBTR_TEMPLATE_USED_BY_GROUP, TC_BLACK, SA_HOR_CENTER);
		}

		/* Draw the number of trains that still need to be treated */
		int num_trains = CountTrainsToReplace(group);
		/* Draw text */
		TextColour color = TC_GREY;
		if ( num_trains ) color = TC_BLACK;
		DrawString(left, right-16, y+2, STR_TBTR_NUM_TRAINS_NEED_RPL, color, SA_RIGHT);
		/* Draw number */
		if ( num_trains ) color = TC_ORANGE;
		else color = TC_GREY;
		SetDParam(0, num_trains);
		DrawString(left, right-4, y+2, STR_JUST_INT, color, SA_RIGHT);

		y += this->line_height / 2;
	}
}

/**
 * Draw template info, like cost, length, etc.
 */
void TbtrGui::DrawTemplateInfo(const Rect &r) const
{
	if ( this->index_selected_template == -1 || (short)this->templates.Length() <= this->index_selected_template )
		return;

	const TemplateVehicle *tmp = this->templates[this->index_selected_template];

	/* Draw vehicle performance info */
	SetDParam(2, tmp->max_speed);
	SetDParam(1, tmp->power);
	SetDParam(0, tmp->weight);
	SetDParam(3, tmp->max_te);
	DrawString(r.left+8, r.right, r.top+4, STR_VEHICLE_INFO_WEIGHT_POWER_MAX_SPEED_MAX_TE);

	/* Draw cargo summary */
	short top = r.top + 24;
	short left = r.left + 8;
	short count_rows = 0;
	short max_rows = 2;

	CargoArray cargo_caps;
	for ( ; tmp; tmp=tmp->Next() )
		cargo_caps[tmp->cargo_type] += tmp->cargo_cap;
	int y = top;
	for (CargoID i = 0; i < NUM_CARGO; ++i) {
		if ( cargo_caps[i] > 0 ) {
			count_rows++;
			SetDParam(0, i);
			SetDParam(1, cargo_caps[i]);
			SetDParam(2, _settings_game.vehicle.freight_trains);
			DrawString(left, r.right, y, FreightWagonMult(i) > 1 ? STR_TBTR_CARGO_SUMMARY_MULTI : STR_TBTR_CARGO_SUMMARY, TC_WHITE, SA_LEFT);
			y += this->line_height/2;
			if ( count_rows % max_rows == 0 ) {
				y = top;
				left += 150;
			}
		}
	}
}

void TbtrGui::DrawTemplates(const Rect& r) const
{
	int left = r.left;
	int right = r.right;
	int y = r.top;

	uint max = min(vscroll_templates->GetPosition() + vscroll_templates->GetCapacity(), this->templates.Length());
	const TemplateVehicle* tv;
	for ( uint i = vscroll_templates->GetPosition(); i<max; ++i)
	{
		tv = (this->templates)[i];

		/* Fill the background of the current cell in a darker tone for the currently selected template */
		if ( this->index_selected_template == (int32)i ) {
			GfxFillRect(left, y, right, y+this->line_height, _colour_gradient[COLOUR_GREY][3]);
		}

		/* Draw a notification string for chains that are not runnable */
		if ( tv->IsFreeWagonChain() ) {
			DrawString(left, right-2, y+this->line_height-FONT_HEIGHT_SMALL-WD_FRAMERECT_BOTTOM - 2, STR_TBTR_WARNING_FREE_WAGON, TC_RED, SA_RIGHT);
		}

		/* Draw the template's length in tile-units */
		SetDParam(0, tv->GetRealLength());
		SetDParam(1, 1);
		DrawString(left, right-4, y+2, STR_TINY_BLACK_DECIMAL, TC_BLACK, SA_RIGHT);

		/* Draw the template */
		tv->Draw(left+50, right, y);

		/* Buying cost */
		SetDParam(0, tv->CalculateCost());
		DrawString(left+35, right, y + this->line_height - FONT_HEIGHT_SMALL - WD_FRAMERECT_BOTTOM - 2, STR_TBTR_TEMPLATE_OVR_VALUE_notinyfont, TC_BLUE, SA_LEFT);

		/* Index of current template vehicle in the list of all templates for its company */
		SetDParam(0, i);
		DrawString(left+5, left+25, y + this->line_height/2, STR_BLACK_INT, TC_BLACK, SA_RIGHT);

		/* Draw whether the current template is in use by any group */
		int n_groups = tv->CountGroups();
		if ( n_groups > 0 )
		{
			SetDParam(0, n_groups);
			DrawString(left+200, right, y + this->line_height - FONT_HEIGHT_SMALL - WD_FRAMERECT_BOTTOM - 2, STR_TBTR_TEMPLATE_IN_USE, TC_GREEN, SA_LEFT);
		}

		/* Draw information about template configuration settings */
		TextColour color;
		if ( tv->IsSetReuseDepotVehicles() ) color = TC_LIGHT_BLUE;
		else color = TC_GREY;
		DrawString(left+200, right, y+2, STR_TBTR_CONFIG_USEDEPOT, color, SA_LEFT);
		if ( tv->IsSetKeepRemainingVehicles() ) color = TC_LIGHT_BLUE;
		else color = TC_GREY;
		DrawString(left+275, right, y+2, STR_TBTR_CONFIG_KEEPREMAINDERS, color, SA_LEFT);
		if ( tv->IsSetRefitAsTemplate() ) color = TC_LIGHT_BLUE;
		else color = TC_GREY;
		DrawString(left+350, right, y+2, STR_TBTR_CONFIG_REFIT, color, SA_LEFT);

		y += this->line_height;
	}
}

int TbtrGui::FindTemplateInGuiList(TemplateID tid) const
{
	for ( uint i=0; i<templates.Length(); ++i )
		if ( templates[i]->index == tid )
			return i;
	return -1;
}

/*
 * Handle mouse clicks on the GUI
 */
void TbtrGui::OnClick(Point pt, int widget, int click_count)
{
	switch (widget) {
		case TRW_WIDGET_TMPL_BUTTONS_CLONE: {
			this->SetWidgetDirty(TRW_WIDGET_TMPL_BUTTONS_CLONE);
			this->ToggleWidgetLoweredState(TRW_WIDGET_TMPL_BUTTONS_CLONE);

			if (this->IsWidgetLowered(TRW_WIDGET_TMPL_BUTTONS_CLONE)) {
				static const CursorID clone_icon =	SPR_CURSOR_CLONE_TRAIN;
				SetObjectToPlaceWnd(clone_icon, PAL_NONE, HT_VEHICLE, this);
			} else {
				ResetObjectToPlace();
			}
			break;
		}
		case TRW_WIDGET_TMPL_BUTTONS_DELETE: {
			TemplateID tid = this->templates[this->index_selected_template]->index;
			bool template_deleted = DoCommandP(0, tid, 0, CMD_DELETE_TEMPLATE);
			if ( template_deleted )
			{
				this->index_selected_template = -1;
			}

			break;
		}
		case TRW_WIDGET_NEW_ENGINES_MATRIX:
		{
		}
		case TRW_WIDGET_TOP_MATRIX:
		{
			uint16 index_new = (uint16)((pt.y - this->nested_array[TRW_WIDGET_TOP_MATRIX]->pos_y) / (this->line_height/2) ) + this->vscroll_groups->GetPosition();
			if ( index_new >= this->groups.Length() )
				this->index_selected_group = -1;
			else if ( this->index_selected_group == index_new )
				this->index_selected_group = -1;
			else
				this->index_selected_group = index_new;
			break;
		}
		case TRW_WIDGET_BOTTOM_MATRIX:
		{
			uint16 index_new = (uint16)((pt.y - this->nested_array[TRW_WIDGET_BOTTOM_MATRIX]->pos_y) / this->line_height ) + this->vscroll_templates->GetPosition();
			if ( index_new >= this->templates.Length() )
				this->index_selected_template = -1;
			else if ( this->index_selected_template == index_new )
				this->index_selected_template = -1;
			else
				this->index_selected_template = index_new;
			break;
		}
		case TRW_WIDGET_START:
		{
			if ( this->index_selected_group>=0 && this->index_selected_template>=0 )
			{
				const TemplateVehicle* tv = *(this->templates.Get(this->index_selected_template));
				const Group* g = *(this->groups.Get(this->index_selected_group));
				DoCommandP(0, g->index | (1 << 16), tv->index, CMD_START_STOP_TBTR);
			}
			break;
		}
		case TRW_WIDGET_STOP:
		{
			if ( this->index_selected_group>=0 )
			{
				DoCommandP(0, this->index_selected_group, 0, CMD_START_STOP_TBTR);
			}
			break;
		}
		case TRW_WIDGET_TMPL_BUTTONS_DEFINE:
		{
			// TODO
			break;
		}
		case TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_REUSE:
		{
			if ( index_selected_template >= 0 && index_selected_template < (int)(this->templates.Length()) ) {
				TemplateID template_index = ((this->templates)[index_selected_template])->index;
				DoCommandP(0, template_index, TBTR_OPT_REUSE_DEPOT_VEHICLES, CMD_TOGGLE_TEMPLATE_OPTION);
			}
			break;
		}
		case TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_KEEP: {
			if ( index_selected_template >= 0 && index_selected_template < (int)(this->templates.Length()) ) {
				TemplateID template_index = ((this->templates)[index_selected_template])->index;
				DoCommandP(0, template_index, TBTR_OPT_KEEP_REMAINDERS, CMD_TOGGLE_TEMPLATE_OPTION);
			}
			break;
		}
		case TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_REFIT:
		{
			if ( index_selected_template >= 0 && index_selected_template < (int)(this->templates.Length()) ) {
				TemplateID template_index = ((this->templates)[index_selected_template])->index;
				DoCommandP(0, template_index, TBTR_OPT_REFIT_VEHICLE, CMD_TOGGLE_TEMPLATE_OPTION);
			}
			break;
		}

	}
	this->SetDirty();
}

/**
 * Extra actions when the window needs to be redrawn
 */
void TbtrGui::OnInvalidateData(int data = 0, bool gui_scope = true)
{
	this->groups.ForceRebuild();
	this->templates.ForceRebuild();
}

/*
 * Draw this GUI
 */
void TbtrGui::OnPaint()
{
	this->BuildTemplateEngineList();
	this->BuildGroupList();
	this->BuildTemplateList();
	this->DrawWidgets();
}

/*
 * Update GUI components on resize
 */
void TbtrGui::OnResize()
{
	/* Top Matrix */
	NWidgetCore* nwi = this->GetWidget<NWidgetCore>(TRW_WIDGET_TOP_MATRIX);
	this->vscroll_groups->SetCapacityFromWidget(this, TRW_WIDGET_TOP_MATRIX);
	nwi->widget_data = (this->vscroll_groups->GetCapacity() << MAT_ROW_START) + (1 << MAT_COL_START);

	/* Bottom Matrix */
	NWidgetCore* nwi2 = this->GetWidget<NWidgetCore>(TRW_WIDGET_BOTTOM_MATRIX);
	this->vscroll_templates->SetCapacityFromWidget(this, TRW_WIDGET_BOTTOM_MATRIX);
	nwi2->widget_data = (this->vscroll_templates->GetCapacity() << MAT_ROW_START) + (1 << MAT_COL_START);
}

/*
 * Handle the selection when a train in the game world has been clicked,
 * This is used for cloning a train into a template vehicle chain.
 *
 * @param train:  pointer to the train that was clicked on, assumes that this is the first vehicle
 *                of the train
 */
bool TbtrGui::OnVehicleSelect(const Vehicle* v)
{
	if (v->type != VEH_TRAIN)
		return false;

	if ( !DoCommandP(0, v->index, 0, CMD_CLONE_TEMPLATE_FROM_TRAIN) )
		return false;

	BuildTemplateList();
	this->ToggleWidgetLoweredState(TRW_WIDGET_TMPL_BUTTONS_CLONE);
	ResetObjectToPlace();
	this->SetDirty();

	return true;
}

/*
 * Show the TBTR Gui
 */
void ShowTbtrGui()
{
	new TbtrGui(&_tbtr_gui_desc);
}

/**
 * Count the number of trains (chains) that need to be treated for a given group
 *
 * @param group:	the group for which we want the count
 * @return:			int, number of chains to be treated, i.e. not the invidual vehicles
 */
int CountTrainsToReplace(const Group* group)
{
	int count = 0;
	if ( group->template_id == INVALID_TEMPLATE )
		return count;
	TemplateVehicle* tv = TemplateVehicle::Get(group->template_id);
	Train* t;
	FOR_ALL_TRAINS(t) {
		if ( t->IsPrimaryVehicle() && t->group_id == group->index && tv && tv->TrainNeedsReplacement(t) )
			++count;
	}
	return count;
}
