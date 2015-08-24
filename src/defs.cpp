
#include <vtapi/common/defs.h>

namespace vtapi {


const std::string def_val_video = "video";
const std::string def_val_images = "images";

const std::string def_fnc_ds_create = "public.VT_dataset_create";
const std::string def_fnc_ds_reset = "public.VT_dataset_truncate";
const std::string def_fnc_ds_delete = "public.VT_dataset_drop";
const std::string def_fnc_nt_create = "public.VT_method_add";
const std::string def_fnc_mt_delete = "public.VT_method_delete";
const std::string def_fnc_task_create = "public.VT_task_create";
const std::string def_fnc_task_delete = "public.VT_task_delete";

const std::string def_tab_datasets = "public.datasets";
const std::string def_tab_methods = "public.methods";
const std::string def_tab_methodkeys = "public.methodkeys";
const std::string def_tab_methodparams = "public.methodparams";
const std::string def_tab_sequences = "sequences";
const std::string def_tab_tasks = "tasks";
const std::string def_tab_processes = "processes";
const std::string def_tab_tasks_prereq = "rel_tasks_tasks_prerequisities";
const std::string def_tab_processes_seq = "rel_processes_sequences_assigned";
const std::string def_tab_tasks_seq = "rel_tasks_sequences_done";


const std::string def_col_all = "*";

const std::string def_col_ds_name = "dsname";
const std::string def_col_ds_fname = "friendly_name";
const std::string def_col_ds_location = "dslocation";
const std::string def_col_ds_description = "description";

const std::string def_col_mt_name = "mtname";
const std::string def_col_mt_description = "description";

const std::string def_col_task_name = "taskname";
const std::string def_col_task_mtname = "mtname";
const std::string def_col_task_params = "params";
const std::string def_col_task_outputs = "outputs";
const std::string def_col_task_created = "created";

const std::string def_col_tprq_taskname = "taskname";
const std::string def_col_tprq_taskprereq = "taskprereq";

const std::string def_col_prss_prsid = "prsid";
const std::string def_col_prss_seqname = "seqname";

const std::string def_col_tsd_taskname = "taskname";
const std::string def_col_tsd_seqname = "seqname";
const std::string def_col_tsd_isdone = "is_done";


const std::string def_col_seq_name = "seqname";
const std::string def_col_seq_location = "seqlocation";
const std::string def_col_seq_type = "seqtyp";
const std::string def_col_seq_vidlength = "vid_length";
const std::string def_col_seq_vidfps = "vid_fps";
const std::string def_col_seq_vidspeed = "vid_speed";
const std::string def_col_seq_vidtime = "vid_time";
const std::string def_col_seq_created = "created";
const std::string def_col_seq_comment = "comment";

const std::string def_col_int_id = "id";
const std::string def_col_int_taskname = "taskname";
const std::string def_col_int_seqname = "seqname";
const std::string def_col_int_t1 = "t1";
const std::string def_col_int_t2 = "t2";
const std::string def_col_int_imglocation = "imglocation";
const std::string def_col_int_rtstart = "rt_start";
const std::string def_col_int_seclength = "sec_length";
const std::string def_col_int_created = "created";

const std::string def_col_prs_prsid = "prsid";
const std::string def_col_prs_taskname = "taskname";
const std::string def_col_prs_state = "state";
const std::string def_col_prs_pid = "ipc_pid";
const std::string def_col_prs_port = "ipc_port";
const std::string def_col_prs_created = "created";

const std::string def_col_prs_pstate_status = "state,status";
const std::string def_col_prs_pstate_progress = "state,progress";
const std::string def_col_prs_pstate_curritem = "state,current_item";
const std::string def_col_prs_pstate_errmsg = "state,last_error";


}
