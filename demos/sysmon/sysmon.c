// SPDX-License-Identifier: GPL-2.0
/*
 * sysmon - retro system monitor (PROJECT SKELETON - fill in the TODOs)
 *
 * Goal: a green-on-black CRT-style dashboard showing live data from
 * procfs - the same interface top(1), free(1) and uptime(1) use:
 * kernel version, uptime, load average, CPU % and memory bars. The
 * LED shows the CPU load at a glance.
 *
 * The retro layout (UNSCII pixel fonts, bars, blinking cursor) and
 * the uptime/loadavg parsing are already written as examples of the
 * fopen/fscanf pattern. You implement:
 *
 *   TODO 1: CPU usage from /proc/stat
 *   TODO 2: memory usage from /proc/meminfo
 *   TODO 3: CPU load LED (green < 30 %, blue < 70 %, red above)
 *
 * ------------------------- API quick reference -------------------------
 * LEDs (common/hal.h):
 *   void hal_leds(uint32_t mask);        bit0=red, bit1=green, bit2=blue
 *
 * LVGL:
 *   lv_label_set_text_fmt(lbl, "cpu %3d%%", v);   note %% for a literal %
 *   lv_bar_set_value(bar, v, LV_ANIM_OFF);        bars go 0..100
 *
 * procfs formats:
 *   /proc/stat, first line ("cpu" = sum of all cores, in clock ticks):
 *     cpu  user nice system idle iowait irq softirq ...
 *   CPU usage must be computed as a *delta* between two reads:
 *     total = user+nice+system+idle+iowait+irq+softirq
 *     usage% = 100 - (idle_delta * 100 / total_delta)
 *   (keep the previous total/idle in static variables; include iowait
 *   in idle. First call has no delta yet - return 0.)
 *
 *   /proc/meminfo (values in kB):
 *     MemTotal:       2013840 kB
 *     MemAvailable:   1704392 kB
 *   used% = (MemTotal - MemAvailable) * 100 / MemTotal
 *
 * Parsing hints:
 *   fscanf(f, "cpu %llu %llu %llu %llu %llu %llu %llu", ...)
 *   fscanf(f, "%63s %ld kB\n", key, &val) in a loop + strcmp(key, ...)
 *
 * -----------------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include "hal.h"

#define GREEN 0x33FF66
#define DARK  0x0A2A12


static lv_obj_t *uptime_lbl, *load_lbl, *cpu_lbl, *mem_lbl, *mem_usg_lbl, *cursor_lbl;

#define ANOMALY_JUMP 25

enum { SCR_HOME, SCR_ANOMALIES, NUM_SCREENS };

static lv_obj_t *screens[NUM_SCREENS];


static lv_obj_t *cpu_bar, *mem_bar;
static lv_obj_t *anomaly_lbl;

static double cpu_baseline = -1, mem_baseline = -1;
static int cpu_anomaly_count, mem_anomaly_count;

static void show_screen(int idx)
{
	for (int i = 0; i < NUM_SCREENS; i++) {
		if (i == idx)
			lv_obj_remove_flag(screens[i], LV_OBJ_FLAG_HIDDEN);
		else
			lv_obj_add_flag(screens[i], LV_OBJ_FLAG_HIDDEN);
	}
}

static void nav_tick(lv_timer_t *t)
{
	LV_UNUSED(t);
	if (hal_button_pressed(HACKPAD_BTN_SW1))
		show_screen(SCR_HOME);
	if (hal_button_pressed(HACKPAD_BTN_SW4))
		show_screen(SCR_ANOMALIES);
}

static void check_anomaly(int value, double *baseline, int *count)
{
	if (value < 0)
		return;

	if (*baseline < 0) {
		*baseline = value;
		return;
	}

	if (value - *baseline >= ANOMALY_JUMP) {
		(*count)++;
		*baseline = value;
	} else {
		*baseline = *baseline * 0.8 + value * 0.2;
	}
}

/* Return the CPU usage in percent, or -1 on error */
static int read_cpu_percent(void)
{
<<<<<<< HEAD
=======

>>>>>>> 77826b6 (anomaly detection on cpu %)
	static unsigned long long prev_total = 0, prev_idle = 0;
	unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
	unsigned long long total, idle_total, total_diff, idle_diff;
	FILE *fp;
	int ret;

	fp = fopen("/proc/stat", "r");
	if (!fp)
		return -1;

	ret = fscanf(fp, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
		     &user, &nice, &system, &idle,
		     &iowait, &irq, &softirq, &steal);
	fclose(fp);

	if (ret != 8)
		return -1;

	// spec: total = user+nice+system+idle+iowait+irq+softirq
	total = user + nice + system + idle + iowait + irq + softirq;

	// in caz ca am ceva io operatii 
	idle_total = idle + iowait;

	if (prev_total == 0 && prev_idle == 0) {
		prev_total = total;
		prev_idle  = idle_total;
		return 0;
	}
	
	total_diff = total - prev_total;
	idle_diff  = idle_total - prev_idle;

	prev_total = total;
	prev_idle  = idle_total;

	if (total_diff == 0)
		return 0;

	return (int)(100 - (idle_diff * 100 / total_diff));
<<<<<<< HEAD
	/* TODO 1: parse the first line of /proc/stat and compute the
	 * usage from the delta with the previous call (see the header
	 * comment for the formula). You need two static variables to
	 * remember the previous total and idle counters.
	 */

	
=======
>>>>>>> 77826b6 (anomaly detection on cpu %)
	return -1;
}

/* Return the memory usage in percent (and MB through the pointers),
 * or -1 on error */
static int read_mem_percent(long *used_mb, long *total_mb)
{
	FILE *fp;
	char key[64];
<<<<<<< HEAD
	char junk[128];
	long int val; 
	long int available = 0, total = 0;
	while (1) {
		fscanf(f, "%s %ld %s", key, &val, junk);
		if (strcmp("MemAvailable:", key) == 0) {
			available = val;
			break;
		} else if (strcmp("MemTotal:", key) == 0) {
			total = val;
		} else if (strcmp("MemFree:", key) == 0) {
			continue;
		}
		
	}
	fclose(f);

	*used_mb = (total - available) / 1024;
	*total_mb = total / 1024;

	return (total - available) * 100 / total;
=======
	long val;
	long total = -1, avail = -1;

	fp = fopen("/proc/meminfo", "r");
	if (!fp)
		return -1;

	while (fscanf(fp, "%63s %ld kB\n", key, &val) == 2) {
		if (strcmp(key, "MemTotal:") == 0)
			total = val;
		else if (strcmp(key, "MemAvailable:") == 0)
			avail = val;

			if (total >= 0 && avail >= 0)
			break;
	}
	fclose(fp);

	if (total <= 0 || avail < 0)
		return -1;

	*used_mb  = (total - avail) / 1024;
	*total_mb = total / 1024;

	return (int)((total - avail) * 100 / total);
>>>>>>> 77826b6 (anomaly detection on cpu %)
}


/* Every second: refresh all the readings */
static void update_tick(lv_timer_t *t)
{
	LV_UNUSED(t);
	char buf[96];

	/* Uptime - given as an example of the procfs pattern */
	FILE *f = fopen("/proc/uptime", "r");
	double up = 0;

	if (f) {
		if (fscanf(f, "%lf", &up) != 1)
			up = 0;
		fclose(f);
	}
	snprintf(buf, sizeof(buf), "up  %02d:%02d:%02d",
		 (int)up / 3600, ((int)up / 60) % 60, (int)up % 60);
	lv_label_set_text(uptime_lbl, buf);

	/* Load average - also given */
	f = fopen("/proc/loadavg", "r");
	if (f) {
		double l1 = 0, l5 = 0, l15 = 0;

		if (fscanf(f, "%lf %lf %lf", &l1, &l5, &l15) == 3) {
			snprintf(buf, sizeof(buf),
				 "load %.2f %.2f %.2f", l1, l5, l15);
			lv_label_set_text(load_lbl, buf);
		}
		fclose(f);
	}

	/* CPU */
	int cpu = read_cpu_percent();

	if (cpu >= 0) {
		lv_label_set_text_fmt(cpu_lbl, "cpu %3d%%", cpu);
		lv_bar_set_value(cpu_bar, cpu, LV_ANIM_OFF);

		/* TODO 3: CPU load LED.
		 * hal_leds(): green below 30 %, blue below 70 %, red
		 * above.
		 */
		 //  *   void hal_leds(uint32_t mask); bit0=red, bit1=green, bit2=blue
		 int led;
		 if (cpu < 30) {
			led = 0b10;
		 } else if (cpu < 70) {
			 led = 0b100;
		 } else {
			led = 0b1;
		 }
		 hal_leds(led);

		check_anomaly(cpu, &cpu_baseline, &cpu_anomaly_count);
	}


	/* Memory */
	long used_mb, total_mb;
	int mem = read_mem_percent(&used_mb, &total_mb);

	if (mem >= 0) {
		lv_label_set_text_fmt(mem_lbl, "mem %3d%%",
				      mem);
		lv_label_set_text_fmt(mem_usg_lbl, "usg %ld/%ldMB",
					used_mb, total_mb);
		lv_bar_set_value(mem_bar, mem, LV_ANIM_OFF);

		check_anomaly(mem, &mem_baseline, &mem_anomaly_count);
	}

	lv_label_set_text_fmt(anomaly_lbl, "ANOMALIES:\nCPU  %d\nMEM  %d",
			       cpu_anomaly_count, mem_anomaly_count);
}
/* Blinking terminal cursor for the retro feel */
static void cursor_tick(lv_timer_t *t)
{
	LV_UNUSED(t);
	static bool on;

	on = !on;
	lv_label_set_text(cursor_lbl, on ? "_" : " ");
}

static lv_obj_t *make_line(lv_obj_t *parent, int y, const lv_font_t *font)
{
	lv_obj_t *l = lv_label_create(parent);

	lv_obj_set_style_text_color(l, lv_color_hex(GREEN), 0);
	lv_obj_set_style_text_font(l, font, 0);
	lv_obj_set_pos(l, 8, y);
	lv_label_set_text(l, "");
	return l;
}

static lv_obj_t *make_bar(lv_obj_t *parent, int y)
{
	lv_obj_t *b = lv_bar_create(parent);

	lv_obj_set_size(b, 224, 10);
	lv_obj_set_pos(b, 8, y);
	lv_bar_set_range(b, 0, 100);
	lv_obj_set_style_bg_color(b, lv_color_hex(DARK), LV_PART_MAIN);
	lv_obj_set_style_bg_color(b, lv_color_hex(GREEN), LV_PART_INDICATOR);
	lv_obj_set_style_radius(b, 2, LV_PART_MAIN);
	lv_obj_set_style_radius(b, 2, LV_PART_INDICATOR);
	return b;
}

int main(void)
{
	struct utsname un;

	hal_init();

	lv_obj_t *scr = lv_screen_active();
	lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
	lv_obj_remove_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_t *home = lv_obj_create(scr);
	lv_obj_set_size(home, 240, 240);
	lv_obj_set_pos(home, 0, 0);
	lv_obj_set_style_bg_color(home, lv_color_black(), 0);
	lv_obj_set_style_border_width(home, 0, 0);
	lv_obj_set_style_radius(home, 0, 0);
	lv_obj_remove_flag(home, LV_OBJ_FLAG_SCROLLABLE);
	screens[SCR_HOME] = home;

	lv_obj_t *anomalies = lv_obj_create(scr);
	lv_obj_set_size(anomalies, 240, 240);
	lv_obj_set_pos(anomalies, 0, 0);
	lv_obj_set_style_bg_color(anomalies, lv_color_black(), 0);
	lv_obj_set_style_border_width(anomalies, 0, 0);
	lv_obj_set_style_radius(anomalies, 0, 0);
	lv_obj_remove_flag(anomalies, LV_OBJ_FLAG_SCROLLABLE);
	screens[SCR_ANOMALIES] = anomalies;

	/* Header: hostname + kernel release */
	uname(&un);
	lv_obj_t *hdr = make_line(home, 8, &lv_font_unscii_16);
	lv_label_set_text_fmt(hdr, "%s@lkss", un.nodename);

	lv_obj_t *krn = make_line(home, 30, &lv_font_unscii_8);
	lv_label_set_text_fmt(krn, "linux %s %s", un.release, un.machine);

	lv_obj_t *sep = make_line(home, 46, &lv_font_unscii_8);
	lv_label_set_text(sep, "------------------------------");

<<<<<<< HEAD
	uptime_lbl = make_line(scr,  62, &lv_font_unscii_16);
	load_lbl   = make_line(scr,  86, &lv_font_unscii_8);
	cpu_lbl    = make_line(scr, 102, &lv_font_unscii_16);
	cpu_bar    = make_bar(scr, 124);
	mem_lbl    = make_line(scr, 144, &lv_font_unscii_16);
	mem_bar    = make_bar(scr, 168);
	mem_usg_lbl= make_line(scr, 188, &lv_font_unscii_16);
	cursor_lbl = make_line(scr, 210, &lv_font_unscii_16);
=======
	uptime_lbl = make_line(home,  62, &lv_font_unscii_16);
	load_lbl   = make_line(home,  86, &lv_font_unscii_16);
	cpu_lbl    = make_line(home, 118, &lv_font_unscii_16);
	cpu_bar    = make_bar(home, 140);
	mem_lbl    = make_line(home, 160, &lv_font_unscii_16);
	mem_bar    = make_bar(home, 182);
	cursor_lbl = make_line(home, 204, &lv_font_unscii_16);
>>>>>>> 77826b6 (anomaly detection on cpu %)

	lv_label_set_text(cpu_lbl, "cpu WAIT");
	lv_label_set_text(mem_lbl, "mem WAIT");

	anomaly_lbl = make_line(anomalies, 8, &lv_font_unscii_16);

	show_screen(SCR_HOME);

	update_tick(NULL);
	lv_timer_create(update_tick, 1000, NULL);
	lv_timer_create(cursor_tick, 500, NULL);
	lv_timer_create(nav_tick, 20, NULL);

	hal_run();
	return 0;
}
