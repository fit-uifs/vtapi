/**
 * @file
 * @brief   Global definitions for VTApi
 *
 * @author   Vojtech Froml, xfroml00 (at) stud.fit.vutbr.cz
 * @author   Tomas Volf, ivolf (at) fit.vutbr.cz
 * 
 * @licence   @ref licence "BUT OPEN SOURCE LICENCE (Version 1)"
 * 
 * @copyright   &copy; 2011 &ndash; 2015, Brno University of Technology
 */

#pragma once

#include <string>

namespace vtapi {


static const std::string& def_val_video = "video";
static const std::string& def_val_images = "images";
static const std::string& def_val_postgres = "postgres";
static const std::string& def_val_sqlite = "sqlite";

static const std::string& def_fnc_ds_create = "public.VT_dataset_create";
static const std::string& def_fnc_ds_reset = "public.VT_dataset_truncate";
static const std::string& def_fnc_ds_delete = "public.VT_dataset_drop";
static const std::string& def_fnc_nt_create = "public.VT_method_add";
static const std::string& def_fnc_mt_delete = "public.VT_method_delete";

static const std::string& def_tab_datasets = "public.datasets";
static const std::string& def_tab_methods = "public.methods";
static const std::string& def_tab_methodkeys = "public.methodkeys";
static const std::string& def_tab_methodparams = "public.methodparams";
static const std::string& def_tab_sequences = "sequences";
static const std::string& def_tab_tasks = "tasks";
static const std::string& def_tab_processes = "processes";
static const std::string& def_tab_intervals = "intervals";
static const std::string& def_tab_tasks_prereq = "rel_tasks_tasks_prerequisities";
static const std::string& def_tab_processes_seq = "rel_processes_sequences_assigned";


static const std::string& def_col_all = "*";

static const std::string& def_col_ds_name = "dsname";
static const std::string& def_col_ds_fname = "friendly_name";
static const std::string& def_col_ds_location = "dslocation";
static const std::string& def_col_ds_description = "description";

static const std::string& def_col_mt_name = "mtname";
static const std::string& def_col_mt_description = "description";

static const std::string& def_col_task_name = "taskname";
static const std::string& def_col_task_mtname = "mtname";
static const std::string& def_col_task_params = "params";
static const std::string& def_col_task_outputs = "outputs";
static const std::string& def_col_task_created = "created";

static const std::string& def_col_tprq_taskname = "taskname";
static const std::string& def_col_tprq_taskprereq = "taskprereq";

static const std::string& def_col_prss_prsid = "prsid";
static const std::string& def_col_prss_seqname = "seqname";

static const std::string& def_col_seq_name = "seqname";
static const std::string& def_col_seq_location = "seqlocation";
static const std::string& def_col_seq_type = "seqtyp";
static const std::string& def_col_seq_vidlength = "vid_length";
static const std::string& def_col_seq_vidfps = "vid_fps";
static const std::string& def_col_seq_vidtime = "vid_time";
static const std::string& def_col_seq_created = "created";
static const std::string& def_col_seq_comment = "comment";

static const std::string& def_col_int_id = "id";
static const std::string& def_col_int_taskname = "taskname";
static const std::string& def_col_int_seqname = "seqname";
static const std::string& def_col_int_t1 = "t1";
static const std::string& def_col_int_t2 = "t2";
static const std::string& def_col_int_imglocation = "imglocation";
static const std::string& def_col_int_rtstart = "rt_start";
static const std::string& def_col_int_seclength = "sec_length";
static const std::string& def_col_int_created = "created";

static const std::string& def_col_prs_prsid = "prsid";
static const std::string& def_col_prs_taskname = "taskname";
static const std::string& def_col_prs_state = "state";
static const std::string& def_col_prs_created = "created";

static const std::string& def_col_prs_pstate_status = "state,status";
static const std::string& def_col_prs_pstate_progress = "state,progress";
static const std::string& def_col_prs_pstate_curritem = "state,current_item";
static const std::string& def_col_prs_pstate_errmsg = "state,last_error";


}