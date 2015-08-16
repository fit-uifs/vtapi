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


extern const std::string def_val_video;
extern const std::string def_val_images;

extern const std::string def_fnc_ds_create;
extern const std::string def_fnc_ds_reset;
extern const std::string def_fnc_ds_delete;
extern const std::string def_fnc_nt_create;
extern const std::string def_fnc_mt_delete;
extern const std::string def_fnc_task_create;
extern const std::string def_fnc_task_delete;

extern const std::string def_tab_datasets;
extern const std::string def_tab_methods;
extern const std::string def_tab_methodkeys;
extern const std::string def_tab_methodparams;
extern const std::string def_tab_sequences;
extern const std::string def_tab_tasks;
extern const std::string def_tab_processes;
extern const std::string def_tab_tasks_prereq;
extern const std::string def_tab_processes_seq;
extern const std::string def_tab_tasks_seq;

extern const std::string def_col_all;

extern const std::string def_col_ds_name;
extern const std::string def_col_ds_fname;
extern const std::string def_col_ds_location;
extern const std::string def_col_ds_description;

extern const std::string def_col_mt_name;
extern const std::string def_col_mt_description;

extern const std::string def_col_task_name;
extern const std::string def_col_task_mtname;
extern const std::string def_col_task_params;
extern const std::string def_col_task_outputs;
extern const std::string def_col_task_created;

extern const std::string def_col_tprq_taskname;
extern const std::string def_col_tprq_taskprereq;

extern const std::string def_col_prss_prsid;
extern const std::string def_col_prss_seqname;

extern const std::string def_col_tsd_taskname;
extern const std::string def_col_tsd_seqname;
extern const std::string def_col_tsd_isdone;

extern const std::string def_col_seq_name;
extern const std::string def_col_seq_location;
extern const std::string def_col_seq_type;
extern const std::string def_col_seq_vidlength;
extern const std::string def_col_seq_vidfps;
extern const std::string def_col_seq_vidspeed;
extern const std::string def_col_seq_vidtime;
extern const std::string def_col_seq_created;
extern const std::string def_col_seq_comment;

extern const std::string def_col_int_id;
extern const std::string def_col_int_taskname;
extern const std::string def_col_int_seqname;
extern const std::string def_col_int_t1;
extern const std::string def_col_int_t2;
extern const std::string def_col_int_imglocation;
extern const std::string def_col_int_rtstart;
extern const std::string def_col_int_seclength;
extern const std::string def_col_int_created;

extern const std::string def_col_prs_prsid;
extern const std::string def_col_prs_taskname;
extern const std::string def_col_prs_state;
extern const std::string def_col_prs_ipcport;
extern const std::string def_col_prs_created;

extern const std::string def_col_prs_pstate_status;
extern const std::string def_col_prs_pstate_progress;
extern const std::string def_col_prs_pstate_curritem;
extern const std::string def_col_prs_pstate_errmsg;


}
