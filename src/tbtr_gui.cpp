/*
 * tbtr_gui.cpp
 * Copyright (C) 2018 streen <streen@gentoobox>
 *
 * Distributed under terms of the MIT license.
 */

#include "tbtr_gui.h"

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
};

static const NWidgetPart _widgets[] = {
	// Title bar
	NWidget(NWID_HORIZONTAL),
		NWidget(WWT_CLOSEBOX, COLOUR_GREY),
		NWidget(WWT_CAPTION, COLOUR_GREY, TRW_CAPTION), SetDataTip(STR_TBTR_RPL_TITLE, STR_TOOLTIP_WINDOW_TITLE_DRAG_THIS),
		NWidget(WWT_SHADEBOX, COLOUR_GREY),
		NWidget(WWT_STICKYBOX, COLOUR_GREY),
	EndContainer(),
	//Top Matrix
	NWidget(NWID_VERTICAL),
		NWidget(WWT_INSET, COLOUR_GREY, TRW_WIDGET_INSET_GROUPS), SetMinimalSize(216,12), SetDataTip(STR_TBTR_MAINGUI_DEFINEDGROUPS, STR_TBTR_MAINGUI_DEFINEDGROUPS), SetResize(1, 0), EndContainer(),
		NWidget(NWID_HORIZONTAL),
			NWidget(WWT_MATRIX, COLOUR_GREY, TRW_WIDGET_TOP_MATRIX), SetMinimalSize(216, 0), SetFill(1, 1), SetDataTip(0x1, STR_REPLACE_HELP_LEFT_ARRAY), SetResize(1, 0), SetScrollbar(TRW_WIDGET_TOP_SCROLLBAR),
			NWidget(NWID_VSCROLLBAR, COLOUR_GREY, TRW_WIDGET_TOP_SCROLLBAR),
		EndContainer(),
	EndContainer(),
	// Template Display
	NWidget(NWID_VERTICAL),
		NWidget(WWT_INSET, COLOUR_GREY, TRW_WIDGET_INSET_TEMPLATES), SetMinimalSize(216,12), SetDataTip(STR_TBTR_AVAILABLE_TEMPLATES, STR_TBTR_AVAILABLE_TEMPLATES), SetResize(1, 0), EndContainer(),
		NWidget(NWID_HORIZONTAL),
			NWidget(WWT_MATRIX, COLOUR_GREY, TRW_WIDGET_BOTTOM_MATRIX), SetMinimalSize(216, 0), SetFill(1, 1), SetDataTip(0x1, STR_REPLACE_HELP_RIGHT_ARRAY), SetResize(1, 1), SetScrollbar(TRW_WIDGET_BOTTOM_SCROLLBAR),
			NWidget(NWID_VSCROLLBAR, COLOUR_GREY, TRW_WIDGET_BOTTOM_SCROLLBAR),
		EndContainer(),
	EndContainer(),
	// Info Area
	NWidget(NWID_VERTICAL),
		NWidget(WWT_INSET, COLOUR_GREY, TRW_WIDGET_TMPL_INFO_INSET), SetMinimalSize(216,12), SetResize(1,0), SetDataTip(STR_TBTR_AVAILABLE_TEMPLATES, STR_TBTR_AVAILABLE_TEMPLATES), EndContainer(),
		NWidget(WWT_PANEL, COLOUR_GREY, TRW_WIDGET_TMPL_INFO_PANEL), SetMinimalSize(216,50), SetResize(1,0), EndContainer(),
	EndContainer(),
	// Control Area
	NWidget(NWID_VERTICAL),
		// Spacing
		NWidget(WWT_INSET, COLOUR_GREY, TRW_WIDGET_TMPL_PRE_BUTTON_FLUFF), SetMinimalSize(139, 12), SetResize(1,0), EndContainer(),
		// Config buttons
		NWidget(NWID_HORIZONTAL),
			NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_REUSE), SetMinimalSize(150,12), SetResize(0,0), SetDataTip(STR_TBTR_SET_USEDEPOT, STR_TBTR_SET_USEDEPOT_TIP),
			NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_KEEP), SetMinimalSize(150,12), SetResize(0,0), SetDataTip(STR_TBTR_SET_KEEPREMAINDERS, STR_TBTR_SET_KEEPREMAINDERS_TIP),
			NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_CONFIGTMPL_REFIT), SetMinimalSize(150,12), SetResize(0,0), SetDataTip(STR_TBTR_SET_REFIT, STR_TBTR_SET_REFIT_TIP),
			NWidget(WWT_PANEL, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_CONFIG_RIGHTPANEL), SetMinimalSize(12,12), SetResize(1,0), EndContainer(),
		EndContainer(),
		// Edit buttons
		NWidget(NWID_HORIZONTAL),
			NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_DEFINE), SetMinimalSize(75,12), SetResize(0,0), SetDataTip(STR_TBTR_DEFINE_TEMPLATE, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
			NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_EDIT), SetMinimalSize(75,12), SetResize(0,0), SetDataTip(STR_TBTR_EDIT_TEMPLATE, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
			NWidget(WWT_TEXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_CLONE), SetMinimalSize(75,12), SetResize(0,0), SetDataTip(STR_TBTR_CREATE_CLONE_VEH, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
			NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_DELETE), SetMinimalSize(75,12), SetResize(0,0), SetDataTip(STR_TBTR_DELETE_TEMPLATE, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
			NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_RPLALL), SetMinimalSize(150,12), SetResize(0,0), SetDataTip(STR_TBTR_RPL_ALL_TMPL, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
			NWidget(WWT_PANEL, COLOUR_GREY, TRW_WIDGET_TMPL_BUTTONS_EDIT_RIGHTPANEL), SetMinimalSize(50,12), SetResize(1,0), EndContainer(),
		EndContainer(),
	EndContainer(),
	// Start/Stop buttons
	NWidget(NWID_HORIZONTAL),
		NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_START), SetMinimalSize(150, 12), SetDataTip(STR_TBTR_RPL_START, STR_REPLACE_ENGINE_WAGON_SELECT_HELP),
		NWidget(WWT_PANEL, COLOUR_GREY, TRW_WIDGET_TRAIN_FLUFF_LEFT), SetMinimalSize(15, 12), EndContainer(),
		NWidget(WWT_DROPDOWN, COLOUR_GREY, TRW_WIDGET_TRAIN_RAILTYPE_DROPDOWN), SetMinimalSize(150, 12), SetDataTip(0x0, STR_REPLACE_HELP_RAILTYPE), SetResize(1, 0),
		NWidget(WWT_PANEL, COLOUR_GREY, TRW_WIDGET_TRAIN_FLUFF_RIGHT), SetMinimalSize(16, 12), EndContainer(),
		NWidget(WWT_PUSHTXTBTN, COLOUR_GREY, TRW_WIDGET_STOP), SetMinimalSize(150, 12), SetDataTip(STR_TBTR_RPL_STOP, STR_REPLACE_REMOVE_WAGON_HELP),
		NWidget(WWT_RESIZEBOX, COLOUR_GREY),
	EndContainer(),
};

static WindowDesc _tbtr_gui_desc(
	WDP_AUTO,
	"TBTR Gui",
	456, 156,
	WC_TBTR_GUI,
	WC_NONE,					// parent window class
	WDF_CONSTRUCTION,
	_widgets, lengthof(_widgets)
);

TbtrGui::TbtrGui(WindowDesc* wdesc) : Window(wdesc)
{
	CreateNestedTree(wdesc);
	this->vscroll[0] = GetScrollbar(TRW_WIDGET_TOP_SCROLLBAR);
	this->vscroll[1] = GetScrollbar(TRW_WIDGET_BOTTOM_SCROLLBAR);
	this->vscroll[0]->SetStepSize(line_height / 2);
	this->vscroll[1]->SetStepSize(line_height);
	FinishInitNested(VEH_TRAIN);

	this->groups.ForceRebuild();
	this->groups.NeedResort();
	this->BuildGroupList(_local_company);
	// TODO impl sorting
	//this->groups.Sort(&GroupNameSorter);
}

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
	}
}

void TbtrGui::BuildGroupList(Owner owner)
{
	if (!this->groups.NeedRebuild()) {
		return;
	}

	this->groups.Clear();
	const Group *g;
	FOR_ALL_GROUPS(g) {
		if (g->owner == owner) {
			*(this->groups).Append() = g;
		}
	}

	this->groups.Compact();
	this->groups.RebuildDone();
}

void TbtrGui::BuildTemplateList(GUITemplateList* list, Scrollbar* vscroll, Owner owner, RailType railtype)
{
	list->Clear();
	const TemplateVehicle *tv;

	FOR_ALL_TEMPLATES(tv) {
		if (tv->HasOwner(owner) && (tv->IsPrimaryVehicle() || tv->IsFreeWagonChain()) && tv->ContainsRailType(railtype))
			*list->Append() = tv;

	}

	list->RebuildDone();
	if (vscroll) vscroll->SetCount(list->Length());
}

void TbtrGui::DrawWidget(const Rect& r, int widget) const
{
	switch(widget) {
		case TRW_WIDGET_TOP_MATRIX: {
			// TODO line height
			this->DrawGroups(10, r);
			break;
		}
	}
}

void TbtrGui::DrawGroups(int line_height, const Rect &r) const
{
	int left = r.left + WD_MATRIX_LEFT;
	int right = r.right - WD_MATRIX_RIGHT;
	int y = r.top;
	int max = min(this->vscroll[0]->GetPosition() + this->vscroll[0]->GetCapacity(), this->groups.Length());

	/* Then treat all groups defined by/for the current company */
	for ( int i=this->vscroll[0]->GetPosition(); i<max; ++i ) {
		const Group *g = (this->groups)[i];
		// TODO rename g_id
		short g_id = g->index;

		/* Fill the background of the current cell in a darker tone for the currently selected template */
		if ( this->index_selected_group == i ) {
			GfxFillRect(left, y, right, y+(this->line_height)/2, _colour_gradient[COLOUR_GREY][3]);
		}

		SetDParam(0, g_id);
		StringID str = STR_GROUP_NAME;
		DrawString(left+30, right, y+2, str, TC_BLACK);

        // TODO enable
		//TemplateReplacement* tr = FindTemplateReplacementForGroup(g_id);

		//if ( tr != NULL ) {
		//	// TODO if template replacement exists for this group
		//	if ( true ) {
		//		SetDParam(0, tr->template_id);
		//		DrawString ( left, right, y+2, STR_TBTR_GROUP_USES_TEMPLATE, TC_BLACK, SA_HOR_CENTER);
		//	}
		//	else {
		//		DrawString ( left, right, y+2, STR_TBTR_TMPLRPL_EX_DIFF_RAILTYPE, TC_SILVER, SA_HOR_CENTER);
		//	}
		//}

		///* Draw the number of trains that still need to be treated by the currently selected template replacement */
		//if ( tr ) {
		//	TemplateVehicle *tv = TemplateVehicle::Get(tr->template_id);
        //    // TODO impl
		//	int num_trains = 0;//NumTrainsNeedTemplateReplacement(g_id, tv);
		//	// Draw text
		//	TextColour color = TC_GREY;
		//	if ( num_trains ) color = TC_BLACK;
		//	DrawString(left, right-16, y+2, STR_TBTR_NUM_TRAINS_NEED_RPL, color, SA_RIGHT);
		//	// Draw number
		//	if ( num_trains ) color = TC_ORANGE;
		//	else color = TC_GREY;
		//	SetDParam(0, num_trains);
		//	DrawString(left, right-4, y+2, STR_JUST_INT, color, SA_RIGHT);
		//}

		y+=line_height / 2;
	}
}

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
    }
    this->SetDirty();
}

void TbtrGui::OnPaint()
{
	BuildGroupList(_local_company);
	DrawWidgets();
}

void TbtrGui::OnResize()
{
    /* Top Matrix */
    NWidgetCore *nwi = this->GetWidget<NWidgetCore>(TRW_WIDGET_TOP_MATRIX);
    this->vscroll[0]->SetCapacityFromWidget(this, TRW_WIDGET_TOP_MATRIX);
    nwi->widget_data = (this->vscroll[0]->GetCapacity() << MAT_ROW_START) + (1 << MAT_COL_START);

    /* Bottom Matrix */
    NWidgetCore *nwi2 = this->GetWidget<NWidgetCore>(TRW_WIDGET_BOTTOM_MATRIX);
    this->vscroll[1]->SetCapacityFromWidget(this, TRW_WIDGET_BOTTOM_MATRIX);
    nwi2->widget_data = (this->vscroll[1]->GetCapacity() << MAT_ROW_START) + (1 << MAT_COL_START);
}

bool TbtrGui::OnVehicleSelect(const Vehicle* v)
{
    // create a new template from the clicked vehicle
    // TODO
    //TemplateVehicle *tv = CloneTemplateVehicleFromTrain((const Train*)v);
    TemplateVehicle* tv;
    if ( !tv )	return false;

    BuildTemplateList(&this->templates, vscroll[1], _local_company, this->railtype);
    this->ToggleWidgetLoweredState(TRW_WIDGET_TMPL_BUTTONS_CLONE);
    ResetObjectToPlace();
    this->SetDirty();

    return true;
}

void ShowTbtrGui()
{
	new TbtrGui(&_tbtr_gui_desc);
}
