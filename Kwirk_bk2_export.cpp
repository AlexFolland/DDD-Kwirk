// To run this instead of a DDD search, add the following line to config.h:
// #define PROBLEM_RELATED Kwirk_bk2_export
// This must be run once for each level, from 0 to 29 consecutively.

#define BIRDS_EYE_VIEW // against intuition, this actually takes less total time, even though it initially adds 1 extra frame; as an added bonus, it's nicer to watch

int export_bk2()
{
	FILE *solution_in;
	{} {            } solution_in = fopen(STRINGIZE(LEVEL)".txt",         "rt");
	if (!solution_in) solution_in = fopen(STRINGIZE(LEVEL)"_bk2.txt",     "rt");
	if (!solution_in) solution_in = fopen(STRINGIZE(LEVEL)"_vbm_mod.txt", "rt");
	if (!solution_in) solution_in = fopen(STRINGIZE(LEVEL)"_vbm.txt",     "rt");
	if (!solution_in)
	{
		fprintf(stderr, "Error reading solution file\n");
		return -1;
	}
	FILE *bk2_out;
	if (LEVEL==0)
		bk2_out = fopen("Kwirk (UA) [optimized]/Input Log.txt", "wt");
	else
		bk2_out = fopen("Kwirk (UA) [optimized]/Input Log.txt", "at");
	if (!bk2_out)
	{
		fclose(solution_in);
		fprintf(stderr, "Error writing bk2 file\n");
		return -1;
	}

	static const char *action_to_bk2[] = {
	/* UP     */ "|U........|\n",
	/* RIGHT  */ "|...R.....|\n",
	/* DOWN   */ "|.D.......|\n",
	/* LEFT   */ "|..L......|\n",
	/* SWITCH */ "|.....s...|\n",
	/* NONE   */ "|.........|\n",
	};
	const char *pressStart = "|....S....|\n";
	const char *pressA     = "|.......A.|\n";

	if (LEVEL == 0)
	{
		fputs(
			"[Input]\n"
			"LogKey:#P1 Up|P1 Down|P1 Left|P1 Right|P1 Start|P1 Select|P1 B|P1 A|P1 Power|\n", bk2_out);

		for (int i=0; i<378; i++)
			fputs(action_to_bk2[NONE], bk2_out);

		fputs(pressStart, bk2_out);

		for (int i=0; i<18; i++)
			fputs(action_to_bk2[NONE], bk2_out);

		// SELECT GAME -> GOING UP
		fputs(pressA, bk2_out);
		for (int i=0; i<16; i++)
			fputs(action_to_bk2[NONE], bk2_out);
		// SELECT SKILL -> EASY
		fputs(pressStart, bk2_out);
		for (int i=0; i<23; i++)
			fputs(action_to_bk2[NONE], bk2_out);
		// SELECT FLOOR -> FL# 1
		fputs(pressA, bk2_out);
		for (int i=0; i<24; i++)
			fputs(action_to_bk2[NONE], bk2_out);
#ifdef BIRDS_EYE_VIEW
		// BIRD'S-EYE VIEW
		fputs(action_to_bk2[DOWN], bk2_out);
#endif
		fputs(action_to_bk2[NONE], bk2_out);
		// SELECT DISPLAY -> DIAGONAL VIEW (or BIRD'S-EYE VIEW)
		fputs(pressA, bk2_out);
		for (int i=0; i<27; i++)
			fputs(action_to_bk2[NONE], bk2_out);
	}

	// GOING UP? -> START
	fputs(pressStart, bk2_out);
	{
		int delay = 167;
		if (LEVEL != 0)
			delay += 1;
		if (LEVEL==3 || LEVEL==15 || LEVEL==19 || LEVEL==20)
			delay += 1;
		else
		if (LEVEL==2 || LEVEL==6)
			delay += 2;
		else
		if (LEVEL==18)
			delay += 3;
		else
		if (LEVEL==5 || LEVEL==10)
			delay += 4;
		else
#ifdef BIRDS_EYE_VIEW
		if (LEVEL==1 || LEVEL==6 || LEVEL==8 || LEVEL==9 || LEVEL==22 || LEVEL==27)
			delay += 1;
#else
		delay += 1;
		if (LEVEL != 0)
			delay += 1;
		if (LEVEL==2 || LEVEL==3 || LEVEL==5 || LEVEL==6 || LEVEL==14 || LEVEL==15 || LEVEL==18 || LEVEL==25 || LEVEL==26 || LEVEL==28)
			delay += 1;
#endif
		for (int i=0; i<delay; i++)
			fputs(action_to_bk2[NONE], bk2_out);
	}

	int frames = 0;
	int steps = -1;
	int switches = 0;
	try
	{
		State state = State::initial;
		Action lastAction = NONE;
		while (state.playersLeft())
		{
			Action action;

			char input[1024];
			fgets(input,sizeof(input),solution_in);
			size_t len=strlen(input);
			if (input[len-1]!='\n') error("Bad input!"); else input[--len]='\0';
			if (input[len-1]=='!' || input[len-1]=='~')
				input[--len]='\0';
			for (action=ACTION_FIRST; action<=NONE; action++)
				if (strcmp(input, actionNames[action])==0)
					goto valid_action;
			error(format("Unrecognized action '%s'", input));
		valid_action:
			steps++;
			if (steps==0 && action!=NONE)
				error("Expected 'None' action");
			if (steps>0 && action==NONE)
				error("Did not expect 'None' action");
			for (int i=0; i<Y; i++)
			{
				fgets(input,sizeof(input),solution_in);
				size_t len=strlen(input);
				if (input[len-1]!='\n') error("Bad input!"); else input[--len]='\0';
				if (input[0] != '#')
					error(format("Unexpected input: '%s'", input));
			}
			if (action==NONE)
				continue;
			if (action==SWITCH)
				switches++;

			int res = state.perform<true,false>(action);
			if (res <= 0)
				error("Bad action!");

#if 1
			if (action==SWITCH)
			{
				int resAdj = res;
				if (lastAction==SWITCH)
				{
					resAdj -= 2;
					for (int i=0; i<2; i++)
						fputs(action_to_bk2[NONE], bk2_out);
				}
				fputs(action_to_bk2[action], bk2_out);
				for (int i=0; i<resAdj-1; i++)
					fputs(action_to_bk2[NONE], bk2_out);
			}
			else
			{
				fputs(action_to_bk2[NONE], bk2_out);
				fputs(action_to_bk2[action], bk2_out);
				for (int i=1; i<res-1; i++)
					fputs(action_to_bk2[NONE], bk2_out);
			}
#else
			for (int i=0; i<res; i++)
				fputs(action_to_bk2[action], bk2_out);
#endif

			lastAction = action;
			frames += res;
		}
		printf("%s", state.toString());
	}
	catch (const char* s)
	{
		puts(s);
	}
	printf("Total %d+%d steps, %d frames (%1.3f seconds)\n", steps-switches, switches, frames, frames/59.7275005696058);

	for (int i=0; i<191; i++)
		fputs(action_to_bk2[NONE], bk2_out);

	fputs(pressStart, bk2_out);

	if (LEVEL == 29)
		fputs("[/Input]\n", bk2_out);
	else
	{
		if (LEVEL % 10 == 9)
		{
			for (int i=0; i<254; i++)
				fputs(action_to_bk2[NONE], bk2_out);
#ifndef BIRDS_EYE_VIEW
			fputs(action_to_bk2[NONE], bk2_out);
#endif

			fputs(pressStart, bk2_out);

			for (int i=0; i<33; i++)
				fputs(action_to_bk2[NONE], bk2_out);
		}
		else
		{
			for (int i=0; i<57; i++)
				fputs(action_to_bk2[NONE], bk2_out);
#ifndef BIRDS_EYE_VIEW
			fputs(action_to_bk2[NONE], bk2_out);
#endif
		}
	}

	fclose(solution_in);
	fclose(bk2_out);
	return 0;
}

int run_related(int argc, const char* argv[])
{
	return export_bk2();
	return 0;
}