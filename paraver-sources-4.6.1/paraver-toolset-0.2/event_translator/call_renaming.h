
struct call_label
{
   char *name;
   unsigned long long type;
   int colour;
   unsigned long long value_ids[100000];
   char *value_name[50000];
   int next_value_slot;
};


