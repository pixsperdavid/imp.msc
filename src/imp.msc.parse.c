// This file is part of imp.msc.
// 
// imp.msc is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published 
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// imp.msc is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with imp.msc.  If not, see <http://www.gnu.org/licenses/>.

#include "ext.h" 
#include "ext_obex.h"
#include "build/version.h"

#define MSC_GO 1
#define MSC_STOP 2
#define MSC_RESUME 3
#define MSC_TIMED_GO 4
#define MSC_LOAD 5
#define MSC_SET 6
#define MSC_FIRE 7
#define MSC_ALL_OFF 8
#define MSC_RESTORE 9
#define MSC_RESET 10
#define MSC_GO_OFF 11
#define MSC_GO_JAM_CLOCK 16
#define MSC_STANDBY_PLUS 17
#define MSC_STANDBY_MINUS 18
#define MSC_SEQUENCE_PLUS 19
#define MSC_SEQUENCE_MINUS 20
#define MSC_START_CLOCK 21
#define MSC_STOP_CLOCK 22
#define MSC_ZERO_CLOCK 23
#define MSC_SET_CLOCK 24
#define MSC_MTC_CHASE_ON 25
#define MSC_MTC_CHASE_OFF 26
#define MSC_OPEN_CUE_LIST 27
#define MSC_CLOSE_CUE_LIST 28
#define MSC_OPEN_CUE_PATH 29
#define MSC_CLOSE_CUE_PATH 30


typedef struct _mscparse
{
	t_object object_; // The object
	void* outletMsc_;
	void* outletDump_;

	t_uint8 buffer_[128];
	t_uint8 bufferPosition_;

	t_uint8 attrDeviceId_;
	t_bool attrDeviceIdAllCall_;
	t_uint8 attrCmdFormat_;
	t_uint8 attrCmdFormatText_;
	t_bool attrCmdFormatAllCall_;
	t_bool attrDeviceGroupEnable_;
	t_uint8 attrDeviceGroup_;
} t_mscparse;


void* mscparse_new(t_symbol* s, long argc, t_atom* argv);
void mscparse_assist(t_mscparse* x, void* b, long m, long a, char* s);

void mscparse_bang(t_mscparse* x);
void mscparse_long(t_mscparse* x, t_atom_long val);
void mscparse_list(t_mscparse* x, t_symbol* s, long argc, t_atom* argv);
void mscparse_parse(t_mscparse* x);
void mscparse_clearbuffer(t_mscparse* x);

void mscparse_c_time_num_list_path(t_mscparse* x);
void mscparse_c_num_list_path(t_mscparse* x);
void mscparse_c_time_list(t_mscparse* x);
void mscparse_c_list(t_mscparse* x);
void mscparse_c_listforce(t_mscparse* x);
void mscparse_c_pathforce(t_mscparse* x);
void mscparse_c_set(t_mscparse* x);
void mscparse_c_fire(t_mscparse* x);
void mscparse_c_none(t_mscparse* x);

int mscparse_buffersearch(t_mscparse* x, int p);

t_max_err mscparse_cmdformattext_get(t_mscparse* x, t_object* attr, long* argc, t_atom** argv);
t_max_err mscparse_cmdformattext_set(t_mscparse* x, t_object* attr, long argc, t_atom* argv);
t_max_err mscparse_devicegroup_enable_set(t_mscparse* x, t_object* attr, long argc, t_atom* argv);
t_max_err mscparse_deviceid_allcall_set(t_mscparse* x, t_object* attr, long argc, t_atom* argv);
t_max_err mscparse_cmdformat_allcall_set(t_mscparse* x, t_object* attr, long argc, t_atom* argv);


t_symbol* _sym_devicegroup;
t_symbol* _sym_deviceid;
t_symbol* _sym_deviceid_allcall;
t_symbol* _sym_cmdformat;
t_symbol* _sym_cmdformattext;

t_symbol* _sym_q_num;
t_symbol* _sym_q_list;
t_symbol* _sym_q_path;


t_class* mscparse_class;


void C74_EXPORT ext_main(void* r)
{
	t_class* c;

	c = class_new("imp.msc.parse", (method)mscparse_new, (method)object_free, (long)sizeof(t_mscparse),
	              0L, A_GIMME, 0);


	class_addmethod(c, (method)mscparse_assist, "assist", A_CANT, 0);
	class_addmethod(c, (method)mscparse_long, "int", A_LONG, 0);
	class_addmethod(c, (method)mscparse_list, "list", A_GIMME, 0);
	class_addmethod(c, (method)mscparse_bang, "bang", A_CANT, 0);
	class_addmethod(c, (method)mscparse_clearbuffer, "clear", 0);


	CLASS_ATTR_CHAR(c, "deviceid", 0, t_mscparse, attrDeviceId_);
	CLASS_ATTR_FILTER_CLIP(c, "deviceid", 0, 111);
	CLASS_ATTR_ORDER(c, "deviceid", 0, "1");
	CLASS_ATTR_LABEL(c, "deviceid", 0, "Individual Device ID") ;
	CLASS_ATTR_SAVE(c, "deviceid", 0);

	CLASS_ATTR_CHAR(c, "deviceid_allcall", 0, t_mscparse, attrDeviceIdAllCall_);
	CLASS_ATTR_STYLE(c, "deviceid_allcall", 0, "onoff");
	CLASS_ATTR_ACCESSORS(c, "deviceid_allcall", 0, mscparse_deviceid_allcall_set);
	CLASS_ATTR_ORDER(c, "deviceid_allcall", 0, "2");
	CLASS_ATTR_LABEL(c, "deviceid_allcall", 0, "All Call Device ID") ;
	CLASS_ATTR_SAVE(c, "deviceid_allcall", 0);

	CLASS_ATTR_CHAR(c, "devicegroup", 0, t_mscparse, attrDeviceGroup_);
	CLASS_ATTR_FILTER_CLIP(c, "devicegroup", 112, 126);
	CLASS_ATTR_ORDER(c, "devicegroup", 0, "3");
	CLASS_ATTR_LABEL(c, "devicegroup", 0, "Group Device ID") ;
	CLASS_ATTR_SAVE(c, "devicegroup", 0);

	CLASS_ATTR_CHAR(c, "devicegroup_enable", 0, t_mscparse, attrDeviceGroupEnable_);
	CLASS_ATTR_STYLE(c, "devicegroup_enable", 0, "onoff");
	CLASS_ATTR_ACCESSORS(c, "devicegroup_enable", 0, mscparse_devicegroup_enable_set);
	CLASS_ATTR_ORDER(c, "devicegroup_enable", 0, "4");
	CLASS_ATTR_LABEL(c, "devicegroup_enable", 0, "Enable Group Device ID") ;
	CLASS_ATTR_SAVE(c, "devicegroup_enable", 0);

	CLASS_ATTR_CHAR(c, "cmdformat", 0, t_mscparse, attrCmdFormat_);
	CLASS_ATTR_FILTER_CLIP(c, "cmdformat", 1, 126);
	CLASS_ATTR_ORDER(c, "cmdformat", 0, "5");
	CLASS_ATTR_LABEL(c, "cmdformat", 0, "Command Format") ;
	CLASS_ATTR_SAVE(c, "cmdformat", 0);

	CLASS_ATTR_CHAR(c, "cmdformat_allcall", 0, t_mscparse, attrCmdFormatAllCall_);
	CLASS_ATTR_STYLE(c, "cmdformat_allcall", 0, "onoff");
	CLASS_ATTR_ACCESSORS(c, "cmdformat_allcall", 0, mscparse_cmdformat_allcall_set);
	CLASS_ATTR_ORDER(c, "cmdformat_allcall", 0, "6");
	CLASS_ATTR_LABEL(c, "cmdformat_allcall", 0, "All Call Command Format") ;
	CLASS_ATTR_SAVE(c, "cmdformat_allcall", 0);

	CLASS_ATTR_CHAR(c, "cmdformattext", 0, t_mscparse, attrCmdFormatText_);
	CLASS_ATTR_ENUMINDEX(c, "cmdformattext", 0, "\"Lighting (General)\" \"Moving Lights\" \"Color Changers\" \"Strobes\" \"Lasers\" \"Chasers\" \"Sound (General)\" \"Music\" \"CD Players\" \"EPROM Playback\" \"Audio Tape Machines\" \"Intercoms\" \"Amplifiers\" \"Audio Effects Devices\" \"Equalizers\" \"Machinery (General)\" \"Rigging\" \"Flies\" \"Lifts\" \"Turntables\" \"Trusses\" \"Robots\" \"Animation\" \"Floats\" \"Breakaways\" \"Barges\" \"Video (General)\" \"Video Tape Machines\" \"Video Cassette Machines\" \"Videodisc Players\" \"Video Switchers\" \"Video Effects\" \"Video Character Generators\" \"Video Still Stores\" \"Video Monitors\" \"Projection (General)\" \"Film Projectors\" \"Slide Projectors\" \"Video Projectors\" \"Dissolvers\" \"Shutter Controls\" \"Process Control (General)\" \"Hydraulic Oil\" \"H20\" \"CO2\" \"Compressed Air\" \"Natural Gas\" \"Fog\" \"Smoke\" \"Cracked Haze\" \"Pyro (General)\" \"Fireworks\" \"Explosions\" \"Flame\" \"Smokepots\" \"Unknown\"");
	CLASS_ATTR_ACCESSORS(c, "cmdformattext", mscparse_cmdformattext_get, mscparse_cmdformattext_set);
	CLASS_ATTR_ORDER(c, "cmdformattext", 0, "7");
	CLASS_ATTR_LABEL(c, "cmdformattext", 0, "Command Format Description") ;
	CLASS_ATTR_SAVE(c, "cmdformattext", 0);


	class_register(CLASS_BOX, c);
	mscparse_class = c;


	_sym_devicegroup = gensym("devicegroup");
	_sym_deviceid = gensym("deviceid");
	_sym_deviceid_allcall = gensym("deviceid_allcall");
	_sym_cmdformat = gensym("cmdformat");
	_sym_cmdformattext = gensym("cmdformattext");

	_sym_q_num = gensym("q_num");
	_sym_q_list = gensym("q_list");
	_sym_q_path = gensym("q_path");


	post("imp.msc.parse V%ld.%ld.%ld by David Butler / The Impersonal Stereo - 03/2016",
		IMP_MSC_VERSION_MAJOR, IMP_MSC_VERSION_MINOR, IMP_MSC_VERSION_BUGFIX);
}




void* mscparse_new(t_symbol* s, long argc, t_atom* argv)
{
	t_mscparse* x = NULL;

	if ((x = (t_mscparse*)object_alloc(mscparse_class)))
	{
		// Create outlets
		x->outletDump_ = outlet_new((t_object *)x, NULL);
		x->outletMsc_ = outlet_new((t_object *)x, NULL);

		// Set up default attributes
		x->attrCmdFormat_ = 1;
		x->attrCmdFormatAllCall_ = FALSE;
		x->attrCmdFormatText_ = 0;
		x->attrDeviceId_ = 0;
		x->attrDeviceGroup_ = 112;
		object_attr_setdisabled((t_object *)x, _sym_devicegroup, 1);
		x->attrDeviceGroupEnable_ = FALSE;

		// Process argument and attributes
		attr_args_process(x, (short)argc, argv);
	}

	return x;
}

void mscparse_assist(t_mscparse* x, void* b, long m, long a, char* s)
{
	if (m == ASSIST_INLET)
	{
		sprintf(s, "connect to sysexin");
	}
	else
	{
		if (a == 0)
			sprintf(s, "msc data");
		if (a == 1)
			sprintf(s, "dump");
	}
}


void mscparse_bang(t_mscparse* x)
{
	// Repeat buffer parsing
	mscparse_parse(x);
}

void mscparse_long(t_mscparse* x, t_atom_long val)
{
	if (val < 0 || val > 255)
	{
		object_error((t_object*)x, "Not a valid MIDI byte");
		return;
	}

	// Test for start or end octet, if false write to buffer
	if (val == 240)
	{
		mscparse_clearbuffer(x);
	}
	else if (val == 247)
	{
		// Write end of buffer value
		x->buffer_[(int)x->bufferPosition_] = (t_uint8)val;

		// Analyse the full message
		mscparse_parse(x);
	}
	else
	{
		x->buffer_[(int)x->bufferPosition_] = (t_uint8)val;
		x->bufferPosition_++;
	}
}

void mscparse_list(t_mscparse* x, t_symbol* s, long argc, t_atom* argv)
{
	for (int i = 0; i < argc; i++)
	{
		if (atom_gettype(argv + i) == A_LONG)
		{
			if (atom_getlong(argv + i) < 0 || atom_getlong(argv + i) > 255)
			{
				object_error((t_object*)x, "List contains items invalid MIDI bytes");
				return;
			}
		}
		else
		{
			object_error((t_object*)x, "List contains items which are not MIDI bytes");
			return;
		}
	}

	for (int i = 0; i < argc; i++)
		mscparse_long(x, atom_getlong(argv + i));
}


void mscparse_clearbuffer(t_mscparse* x)
{
	memset(x->buffer_, 0, 128);
	x->bufferPosition_ = 0;
}


void mscparse_parse(t_mscparse* x)
{
	// Test for MSC message
	if (x->buffer_[0] != 127 || x->buffer_[2] != 2)
		return;

	// Test for device ID or all call setting
	if ((x->buffer_[1] != x->attrDeviceId_) && (x->buffer_[1] != 127) && (x->attrDeviceIdAllCall_ == FALSE))
		return;

	// Test for command format or all call setting
	if ((x->buffer_[3] != x->attrCmdFormat_) && (x->buffer_[3] != 127) && (x->attrCmdFormatAllCall_ == FALSE))
		return;

	// Test for command type and run relevant parsing function
	switch(x->buffer_[4])
	{
	case MSC_GO:
	case MSC_STOP:
	case MSC_RESUME:
	case MSC_LOAD:
	case MSC_GO_OFF:
	case MSC_GO_JAM_CLOCK:
		mscparse_c_num_list_path(x);
		break;

	case MSC_TIMED_GO:
		mscparse_c_time_num_list_path(x);
		break;

	case MSC_SET:
		mscparse_c_set(x);
		break;

	case MSC_FIRE:
		mscparse_c_fire(x);
		break;

	case MSC_ALL_OFF:
	case MSC_RESTORE:
	case MSC_RESET:
		mscparse_c_none(x);
		break;

	case MSC_STANDBY_PLUS:
	case MSC_STANDBY_MINUS:
	case MSC_SEQUENCE_PLUS:
	case MSC_SEQUENCE_MINUS:
	case MSC_START_CLOCK:
	case MSC_STOP_CLOCK:
	case MSC_ZERO_CLOCK:
	case MSC_MTC_CHASE_ON:
	case MSC_MTC_CHASE_OFF:
		mscparse_c_list(x);
		break;

	case MSC_SET_CLOCK:
		mscparse_c_time_list(x);
		break;

	case MSC_OPEN_CUE_LIST:
	case MSC_CLOSE_CUE_LIST:
		mscparse_c_listforce(x);
		break;

	case MSC_OPEN_CUE_PATH:
	case MSC_CLOSE_CUE_PATH:
		mscparse_c_pathforce(x);
		break;

	default:
		object_error((t_object*)x, "Not a valid MSC command type");
		break;
	}
}


void mscparse_c_time_num_list_path(t_mscparse* x)
{
	t_atom argv[12];

	switch(x->buffer_[5] >> 5)
	{
	case 0:
		atom_setsym(argv, gensym("time_24f"));
		break;
	case 1:
		atom_setsym(argv, gensym("time_25f"));
		break;
	case 2:
		atom_setsym(argv, gensym("time_30df"));
		break;
	case 3:
		atom_setsym(argv, gensym("time_30f"));
		break;
	default:
		assert(0);
		return;
	}

	atom_setlong(argv + 1, x->buffer_[5] - (x->buffer_[5] & 96));
	atom_setlong(argv + 2, x->buffer_[6]);
	atom_setlong(argv + 3, x->buffer_[7]);
	atom_setlong(argv + 4, x->buffer_[8]);
	atom_setlong(argv + 5, x->buffer_[9]);


	t_atom_float qnum = -1;
	t_atom_float qlist = -1;
	t_atom_float qpath = -1;

	int search = -1;

	if (x->buffer_[10] != 247)
	{
		qnum = atof((char*)x->buffer_ + 10);

		search = mscparse_buffersearch(x, 10);
		if (search != -1)
		{
			qlist = atof((char*)x->buffer_ + search);

			search = mscparse_buffersearch(x, search);
			if (search != -1)
				qpath = atof((char*)x->buffer_ + search);
		}
	}

	while (true)
	{
		if (qnum == -1)
		{
			outlet_anything(x->outletMsc_, gensym("TIMED_GO"), 6, argv);
			break;
		}
		else
		{
			atom_setsym(argv + 6, _sym_q_num);
			if (floorl(qnum) == qnum)
				atom_setlong(argv + 7, (t_atom_long)floorl(qnum));
			else
				atom_setfloat(argv + 7, qnum);
		}

		if (qlist == -1)
		{
			outlet_anything(x->outletMsc_, gensym("TIMED_GO"), 8, argv);
			break;
		}
		else
		{
			atom_setsym(argv + 8, _sym_q_list);
			if (floorl(qlist) == qlist)
				atom_setlong(argv + 9, (t_atom_long)floorl(qlist));
			else
				atom_setfloat(argv + 9, qlist);
		}

		if (qpath == -1)
		{
			outlet_anything(x->outletMsc_, gensym("TIMED_GO"), 10, argv);
			break;
		}
		else
		{
			atom_setsym(argv + 10, _sym_q_path);
			if (floorl(qpath) == qpath)
				atom_setlong(argv + 11, (t_atom_long)floorl(qpath));
			else
				atom_setfloat(argv + 11, qpath);

			outlet_anything(x->outletMsc_, gensym("TIMED_GO"), 12, argv);
			break;
		}
	}
}

void mscparse_c_num_list_path(t_mscparse* x)
{
	t_atom_float qnum = -1;
	t_atom_float qlist = -1;
	t_atom_float qpath = -1;

	t_symbol* type;

	switch (x->buffer_[4])
	{
	case MSC_GO:
		type = gensym("GO");
		break;
	case MSC_STOP:
		type = gensym("STOP");
		break;
	case MSC_RESUME:
		type = gensym("RESUME");
		break;
	case MSC_LOAD:
		type = gensym("LOAD");
		break;
	case MSC_GO_OFF:
		type = gensym("GO_OFF");
		break;
	case MSC_GO_JAM_CLOCK:
		type = gensym("GO/JAM_CLOCK");
		break;
	default:
		assert(0);
		return;
	}

	int search = -1;

	if (x->buffer_[5] != 247)
	{
		qnum = atof((char*)x->buffer_ + 5);

		search = mscparse_buffersearch(x, 5);
		if (search != -1)
		{
			qlist = atof((char*)x->buffer_ + search);

			search = mscparse_buffersearch(x, search);
			if (search != -1)
				qpath = atof((char*)x->buffer_ + search);
		}
	}

	t_atom argv[6];

	while (1)
	{
		if (qnum == -1)
		{
			atom_setsym(argv, type);
			outlet_atoms(x->outletMsc_, 1, argv);
			break;
		}
		else
		{
			atom_setsym(argv, _sym_q_num);
			if (floorl(qnum) == qnum)
				atom_setlong(argv + 1, (t_atom_long)floorl(qnum));
			else
				atom_setfloat(argv + 1, qnum);
		}

		if (qlist == -1)
		{
			outlet_anything(x->outletMsc_, type, 2, argv);
			break;
		}
		else
		{
			atom_setsym(argv + 2, _sym_q_list);
			if (floorl(qlist) == qlist)
				atom_setlong(argv + 3, (t_atom_long)floorl(qlist));
			else
				atom_setfloat(argv + 3, qlist);
		}

		if (qpath == -1)
		{
			outlet_anything(x->outletMsc_, type, 4, argv);
			break;
		}
		else
		{
			atom_setsym(argv + 4, _sym_q_path);
			if (floorl(qpath) == qpath)
				atom_setlong(argv + 5, (t_atom_long)floorl(qpath));
			else
				atom_setfloat(argv + 5, qpath);

			outlet_anything(x->outletMsc_, type, 6, argv);
			break;
		}
	}
}

void mscparse_c_time_list(t_mscparse* x)
{
	t_atom_float qlist = -1;

	t_atom argv[8];

	switch (x->buffer_[5] >> 5)
	{
	case 0:
		atom_setsym(argv, gensym("time_24f"));
		break;
	case 1:
		atom_setsym(argv, gensym("time_25f"));
		break;
	case 2:
		atom_setsym(argv, gensym("time_30df"));
		break;
	case 3:
		atom_setsym(argv, gensym("time_30f"));
		break;
	default:
		assert(0);
		return;
	}

	atom_setlong(argv + 1, x->buffer_[5] - (x->buffer_[5] & 96));
	atom_setlong(argv + 2, x->buffer_[6]);
	atom_setlong(argv + 3, x->buffer_[7]);
	atom_setlong(argv + 4, x->buffer_[8]);
	atom_setlong(argv + 5, x->buffer_[9]);

	if (x->buffer_[10] != 247)
		qlist = atof((char*)x->buffer_ + 10);

	if (qlist == -1)
	{
		outlet_anything(x->outletMsc_, gensym("SET_CLOCK"), 6, argv);
	}
	else
	{
		atom_setsym(argv + 6, _sym_q_list);

		if (floorl(qlist) == qlist)
			atom_setlong(argv + 7, (t_atom_long)floorl(qlist));
		else
			atom_setfloat(argv + 7, qlist);

		outlet_anything(x->outletMsc_, gensym("SET_CLOCK"), 8, argv);
	}
}

void mscparse_c_list(t_mscparse* x)
{
	t_atom_float qlist = -1;

	t_symbol* type;

	switch (x->buffer_[4])
	{
	case MSC_STANDBY_PLUS:
		type = gensym("STANDBY_+");
		break;
	case MSC_STANDBY_MINUS:
		type = gensym("STANDBY_-");
		break;
	case MSC_SEQUENCE_PLUS:
		type = gensym("SEQUENCE_+");
		break;
	case MSC_SEQUENCE_MINUS:
		type = gensym("SEQUENCE_-");
		break;
	case MSC_START_CLOCK:
		type = gensym("START_CLOCK");
		break;
	case MSC_STOP_CLOCK:
		type = gensym("STOP_CLOCK");
		break;
	case MSC_ZERO_CLOCK:
		type = gensym("ZERO_CLOCK");
		break;
	case MSC_MTC_CHASE_ON:
		type = gensym("MTC_CHASE_ON");
		break;
	case MSC_MTC_CHASE_OFF:
		type = gensym("MTC_CHASE_OFF");
		break;
	default:
		assert(0);
		return;
	}

	t_atom argv[2];

	if (x->buffer_[5] != 247)
		qlist = atof((char*)x->buffer_ + 5);

	if (qlist == -1)
	{
		atom_setsym(argv, type);
		outlet_atoms(x->outletMsc_, 1, argv);
	}
	else
	{
		atom_setsym(argv, _sym_q_list);

		if (floorl(qlist) == qlist)
			atom_setlong(argv + 1, (t_atom_long)floorl(qlist));
		else
			atom_setfloat(argv + 1, qlist);

		outlet_anything(x->outletMsc_, type, 2, argv);
	}
}

void mscparse_c_listforce(t_mscparse* x)
{
	t_atom_float qlist;

	t_symbol* type;

	if (x->buffer_[4] == MSC_OPEN_CUE_LIST)
	{
		type = gensym("OPEN_CUE_LIST");
	}
	else if (x->buffer_[4] == MSC_CLOSE_CUE_LIST)
	{
		type = gensym("CLOSE_CUE_LIST");
	}
	else
	{
		assert(0);
		return;
	}

	if (x->buffer_[5] != 247)
	{
		qlist = atof((char*)x->buffer_ + 5);

		t_atom argv[2];

		atom_setsym(argv, _sym_q_list);

		if (floorl(qlist) == qlist)
			atom_setlong(argv + 1, (t_atom_long)floorl(qlist));
		else
			atom_setfloat(argv + 1, qlist);

		outlet_anything(x->outletMsc_, type, 2, argv);
	}
	else
	{
		return;
	}
}

void mscparse_c_pathforce(t_mscparse* x)
{
	t_atom_float qpath;

	t_symbol* type;

	if (x->buffer_[4] == MSC_OPEN_CUE_PATH)
	{
		type = gensym("OPEN_CUE_PATH");
	}
	else if (x->buffer_[4] == MSC_CLOSE_CUE_PATH)
	{
		type = gensym("CLOSE_CUE_PATH");
	}
	else
	{
		assert(0);
		return;
	}

	if (x->buffer_[5] != 247)
	{
		qpath = atof((char*)x->buffer_ + 5);

		t_atom argv[2];

		atom_setsym(argv, _sym_q_path);

		if (floorl(qpath) == qpath)
			atom_setlong(argv + 1, (t_atom_long)floorl(qpath));
		else
			atom_setfloat(argv + 1, qpath);

		outlet_anything(x->outletMsc_, type, 2, argv);
	}
	else
	{
		return;
	}
}

void mscparse_c_set(t_mscparse* x)
{
	int control = x->buffer_[5] + (x->buffer_[6] << 7);
	int value = x->buffer_[7] + (x->buffer_[8] << 7);

	t_atom argv[10];

	atom_setsym(argv, gensym("control"));
	atom_setlong(argv + 1, control);
	atom_setsym(argv + 2, gensym("value"));
	atom_setlong(argv + 3, value);

	if (x->buffer_[9] == 247)
	{
		outlet_anything(x->outletMsc_, gensym("SET"), 4, argv);
	}
	else
	{
		switch (x->buffer_[9] >> 5)
		{
		case 0:
			atom_setsym(argv, gensym("time_24f"));
			break;
		case 1:
			atom_setsym(argv, gensym("time_25f"));
			break;
		case 2:
			atom_setsym(argv, gensym("time_30df"));
			break;
		case 3:
			atom_setsym(argv, gensym("time_30f"));
			break;
		default:
			assert(0);
			return;
		}

		atom_setlong(argv + 5, x->buffer_[9] - (x->buffer_[9] & 96));
		atom_setlong(argv + 6, x->buffer_[10]);
		atom_setlong(argv + 7, x->buffer_[11]);
		atom_setlong(argv + 8, x->buffer_[12]);
		atom_setlong(argv + 9, x->buffer_[13]);

		outlet_anything(x->outletMsc_, gensym("SET"), 10, argv);
	}
}

void mscparse_c_fire(t_mscparse* x)
{
	int macro = x->buffer_[5] & 127;

	t_atom argv[2];
	atom_setsym(argv, gensym("macro"));
	atom_setlong(argv + 1, macro);
	outlet_anything(x->outletMsc_, gensym("FIRE"), 2, argv);
}

void mscparse_c_none(t_mscparse* x)
{
	t_atom argv[1];
	t_symbol* type;

	switch (x->buffer_[4])
	{
	case MSC_ALL_OFF:
		type = gensym("ALL_OFF");
		break;
	case MSC_RESTORE:
		type = gensym("RESTORE");
		break;
	case MSC_RESET:
		type = gensym("RESET");
		break;
	default:
		assert(0);
		return;
	}

	atom_setsym(argv, type);
	outlet_atoms(x->outletMsc_, 1, argv);
}


int mscparse_buffersearch(t_mscparse* x, int p)
{
	// Search the buffer for the next field, if there is no more information return -1

	int i = p;

	while (i < 128)
	{
		if ((x->buffer_[i] == 0 && x->buffer_[i + 1] == 0) || x->buffer_[i] == 247)
			return -1;
		else if (x->buffer_[i] == 0)
			return i + 1;

		i++;
	}

	return -1;
}


t_max_err mscparse_cmdformattext_get(t_mscparse* x, t_object* attr, long* argc, t_atom** argv)
{
	const int num2text[128] =
		{-1, 0, 1, 2, 3, 4, 5, 56, 56, 56,
			56, 56, 56, 56, 56, 56, 6, 7, 8, 9,
			156, 11, 12, 13, 14, 56, 56, 56, 56, 56,
			56, 56, 15, 16, 17, 18, 19, 256, 21, 22,
			23, 24, 25, 56, 56, 56, 56, 56, 26, 27,
			28, 29, 356, 31, 32, 33, 34, 56, 56, 56,
			56, 56, 56, 56, 35, 36, 37, 38, 39, 456,
			56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
			41, 42, 43, 44, 45, 46, 47, 48, 49, 56,
			56, 56, 56, 56, 56, 56, 556, 51, 52, 53,
			54, 56, 56, 56, 56, 56, 56, 56, 56, 56,
			56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
			56, 56, 56, 56, 56, 56, 56, -1};

	char alloc;

	atom_alloc(argc, argv, &alloc);
	atom_setlong(*argv, num2text[(long)x->attrCmdFormat_]);

	return MAX_ERR_NONE;
}

t_max_err mscparse_cmdformattext_set(t_mscparse* x, t_object* attr, long argc, t_atom* argv)
{
	const int text2num[56] =
		{1, 2, 3, 4, 5, 6,
			16, 17, 18, 19, 20, 21, 22, 23, 24,
			32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
			48, 49, 50, 51, 52, 53, 54, 55, 56,
			64, 65, 66, 67, 68, 69,
			80, 81, 82, 83, 84, 85, 86, 87, 88,
			96, 97, 98, 99, 100, -1};

	x->attrCmdFormatText_ = (t_uint8)atom_getlong(argv);

	x->attrCmdFormat_ = text2num[atom_getlong(argv)];

	return MAX_ERR_NONE;
}

t_max_err mscparse_devicegroup_enable_set(t_mscparse* x, t_object* attr, long argc, t_atom* argv)
{
	x->attrDeviceGroupEnable_ = atom_getlong(argv) > 0 ? true : false;

	if (x->attrDeviceGroupEnable_ == FALSE)
	{
		object_attr_setdisabled((t_object *)x, _sym_devicegroup, 1);

		if (x->attrDeviceIdAllCall_ == FALSE)
			object_attr_setdisabled((t_object *)x, _sym_deviceid, 0);
		else
			object_attr_setdisabled((t_object *)x, _sym_deviceid, 1);

		object_attr_setdisabled((t_object *)x, _sym_deviceid_allcall, 0);
	}
	else
	{
		object_attr_setdisabled((t_object *)x, _sym_devicegroup, 0);
		object_attr_setdisabled((t_object *)x, _sym_deviceid, 1);
		object_attr_setdisabled((t_object *)x, _sym_deviceid_allcall, 1);
	}

	return MAX_ERR_NONE;
}

t_max_err mscparse_deviceid_allcall_set(t_mscparse* x, t_object* attr, long argc, t_atom* argv)
{
	x->attrDeviceIdAllCall_ = atom_getlong(argv) > 0 ? true : false;

	if (x->attrDeviceIdAllCall_ == FALSE)
	{
		if (x->attrDeviceGroupEnable_ == FALSE)
			object_attr_setdisabled((t_object *)x, _sym_deviceid, 0);
		else
			object_attr_setdisabled((t_object *)x, _sym_deviceid, 1);
	}
	else
	{
		object_attr_setdisabled((t_object *)x, _sym_deviceid, 1);
	}

	return MAX_ERR_NONE;
}

t_max_err mscparse_cmdformat_allcall_set(t_mscparse* x, t_object* attr, long argc, t_atom* argv)
{
	x->attrCmdFormatAllCall_ = atom_getlong(argv) > 0 ? true : false;

	if (x->attrCmdFormatAllCall_ == FALSE)
	{
		object_attr_setdisabled((t_object *)x, _sym_cmdformat, 0);
		object_attr_setdisabled((t_object *)x, _sym_cmdformattext, 0);
	}
	else
	{
		object_attr_setdisabled((t_object *)x, _sym_cmdformat, 1);
		object_attr_setdisabled((t_object *)x, _sym_cmdformattext, 1);
	}

	return MAX_ERR_NONE;
}

