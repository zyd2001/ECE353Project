#include "main.h"

typedef struct
{
	Position pos;
	uint8_t speed;
	int16_t hit_point;
} Jet;

typedef struct
{
	Position pos;
	Direction direction;
	uint8_t speed;
	BulletType type;
	bool enemy;
} Bullet;

static Bullet bullets[BULLET_LIMIT];
static Jet enemies[ENEMY_LIMIT];
static int bullet_count = 0;
static int enemy_count = 0;
static int point = 0;
static int killed = 0;
static Position point_pos = {140, 10};
static Jet player = {{120, 300}, 7, 60};
static BulletType bullet_type;
static int bomb = 0;
static bool can_touch = true;
static uint8_t difficulty = 0;
static bool boss_fight = false;
static bool boss_spawned = false;
static Direction boss_direction;
static Jet boss;
static int16_t accel_x, accel_y;

volatile int enemy_spawn_time;

bool screen_touched()
{
	if (can_touch && ft6x06_read_td_status() > 0)
	{
		can_touch = false;
		return true;
	}
	return false;
}

static void clear()
{
	bullet_count = 0;
	enemy_count = 0;
	point = 0;
	player.hit_point = 30;
	player.pos.x = 120;
	player.pos.y = 300;
	bullet_type = NORMAL;
	bomb = 0;
	can_touch = true;
	difficulty = 0;
	boss_fight = false;
	boss_spawned = false;
	enemy_spawn_time = 2400;
	killed = 0;
}

static uint16_t get_player_color()
{
	if (player.hit_point > 10 && player.hit_point <= 20)
		return 0xd800;
	if (player.hit_point <= 10)
		return 0xa000;
	return LCD_COLOR_RED;
}

static void print_score()
{
	char p[15];
	snprintf(p, 15, "%09d", point);
	draw_rectangle_centered(180, 120, 8, 16, LCD_COLOR_BLACK);
	draw_digit(point_pos, p, LCD_COLOR_RED, LCD_COLOR_BLACK);
}

static void remove_bullet(int index)
{
	if (bullets[index].pos.y > 20)
		erase_bullet(bullets[index].pos, bullets[index].type);
	bullets[index] = bullets[bullet_count - 1];
	bullet_count--;
}

static void remove_jet(int index)
{
	if (enemies[index].pos.y < 300)
		erase_jet(enemies[index].pos);
	enemies[index] = enemies[enemy_count - 1];
	enemy_count--;
}

static void win_boss_fight()
{
	erase_boss(boss.pos);
	boss_fight = false;
	point += 2000;
	print_score();
	difficulty++;
}

static void clear_score_board()
{
	eeprom_byte_write(EEPROM_I2C_BASE, 0, 0);
}

static void draw_score_board()
{
	uint8_t entries, t;
	Position pos = {20, 20};
	char str[15];
	int i, score[10];
	set_led(0);
	lcd_clear_screen(LCD_COLOR_BLACK);
	eeprom_byte_read(EEPROM_I2C_BASE, 0, &entries);
	for (i = 0; i < entries; i++)
	{
		score[i] = 0;
		eeprom_byte_read(EEPROM_I2C_BASE, i * 4 + 0 + 1, &t);
		score[i] |= t << 24;
		eeprom_byte_read(EEPROM_I2C_BASE, i * 4 + 1 + 1, &t);
		score[i] |= t << 16;
		eeprom_byte_read(EEPROM_I2C_BASE, i * 4 + 2 + 1, &t);
		score[i] |= t << 8;
		eeprom_byte_read(EEPROM_I2C_BASE, i * 4 + 3 + 1, &t);
		score[i] |= t << 0;
	}
	if (point > 0)
	{
		if (entries < 10)
		{
			score[entries] = point;
			entries++;
		}
		qsort(score, entries, sizeof(int), compare);
		
		if (entries == 10 && score[0] < point)
		{
			for (i = 0; i < entries; i++)
			{
				if (score[i] >= point)
				{
					score[i - 1] = point;
					break;
				}
			}
		}
	}	
	
	eeprom_byte_write(EEPROM_I2C_BASE, 0, entries);
	for (i = entries - 1; i >= 0; i--)
	{
		eeprom_byte_write(EEPROM_I2C_BASE, i * 4 + 0 + 1, score[i] >> 24);
		eeprom_byte_write(EEPROM_I2C_BASE, i * 4 + 1 + 1, score[i] >> 16);
		eeprom_byte_write(EEPROM_I2C_BASE, i * 4 + 2 + 1, score[i] >> 8);
		eeprom_byte_write(EEPROM_I2C_BASE, i * 4 + 3 + 1, score[i] >> 0);
		snprintf(str, 15, "%09d", score[i]);
		draw_digit(pos, str, LCD_COLOR_RED, LCD_COLOR_BLACK);
		pos.y += 25;
	}
	lcd_draw_image(pos.x + 45, startWidthPixels, 290, startHeightPixels, startBitmaps, LCD_COLOR_RED, LCD_COLOR_BLACK);
	lcd_draw_image(180, clearWidthPixels, 160, clearHeightPixels, clearBitmaps, LCD_COLOR_RED, LCD_COLOR_BLACK);
}

static int collision()
{
	int i, damage;
	for (i = 0; i < bullet_count; i++)
	{
		switch (bullets[i].type)
		{
			case NORMAL:
				damage = 8;
				break;
			case BIG:
				damage = 30;
				break;
			case SCATTER:
				damage = 4;
				break;
		}
		if (bullets[i].enemy)
		{
			if (in_range(bullets[i].pos, player.pos, 20, 20))
			{
				player.hit_point -= 10;
				remove_bullet(i);
				i--;
				draw_jet(player.pos, player.pos, get_player_color());
				if (player.hit_point <= 0)
					return 1;
				continue;
			}
		}
		else
		{
			int j;
			if (boss_fight && boss_spawned)
			{
				if (in_range(bullets[i].pos, boss.pos, 42, 42))
				{
					if (bullets[i].type == BIG)
						damage = 24;
					boss.hit_point -= damage;
					remove_bullet(i);
					i--;
					if (boss.hit_point <= 0)
						win_boss_fight();
					continue;
				}
			}
			for (j = 0; j < enemy_count; j++)
			{
				if (in_range(bullets[i].pos, enemies[j].pos, 30, 30))
				{
					enemies[j].hit_point -= damage;
					remove_bullet(i);
					i--;
					
					if (enemies[j].hit_point <= 0)
					{
						point += 100;
						killed += 1;
						print_score();
						remove_jet(j);
						j--;
					}
					break;
				}
			}
		}
	}
	for (i = 0; i < enemy_count; i++)
	{
		if (in_range(enemies[i].pos, player.pos, 50, 50))
		{
			enemies[i].hit_point -= 30;
			player.hit_point -= 15;
			draw_jet(player.pos, player.pos, get_player_color());
			if (enemies[i].hit_point <= 0)
			{
				point += 100;
				killed += 1;
				print_score();
				remove_jet(i);
				i--;
			}
			if (player.hit_point <= 0)
				return 1;
		}
	}
	if (boss_fight)
	{
		if (in_range(player.pos, boss.pos, 40, 40))
		{
			return 1;
		}
	}
	return 0;
}

static void spawn_enemy()
{
	if (boss_fight && !boss_spawned)
	{
		boss.hit_point = 150 + difficulty * 100;
		boss.pos.x = 120;
		boss.pos.y = 50;
		boss.speed = difficulty + 3;
		boss_direction = random(D_LEFT, D_RIGHT);
		boss_spawned = true;
	}
	else if (!boss_fight)
	{
		if (enemy_count < ENEMY_LIMIT)
		{
			Jet j = {{0, 30}, 2, 30};
			j.pos.x = random(20, 220);
			enemies[enemy_count] = j;
			enemy_count++;
		}
	}
}

static void enemy_update()
{
	int i;
	Position new_pos;
	for (i = 0; i < enemy_count; i++)
	{
		new_pos = enemies[i].pos;
		new_pos.y += enemies[i].speed;
		if (new_pos.y < 300)
		{
			draw_enemy_jet(new_pos, enemies[i].pos, LCD_COLOR_BLUE);
			enemies[i].pos = new_pos;
		}
		else
		{
			remove_jet(i);
			i--;
		}
	}
	if (boss_fight)
	{
		new_pos = boss.pos;
		if (boss_direction == D_RIGHT)
			new_pos.x += boss.speed;
		else
			new_pos.x -= boss.speed;
		if (new_pos.x > 170)
			boss_direction = D_LEFT;
		if (new_pos.x < 50)
			boss_direction = D_RIGHT;
		if (new_pos.x > 30 && new_pos.x < 210)
		{
			draw_boss(new_pos, boss.pos, LCD_COLOR_BLUE);
			boss.pos = new_pos;
		}
	}
}

static void enemy_bullet()
{
	static int delay = 0;
	int i;
	Bullet b = {{0, 0}, D_DOWN, 8, NORMAL, true};
	delay++;
	if (delay > 5 - difficulty / 2)
	{
		delay = 0;
		for (i = 0; i < enemy_count; i++)
		{
			if (bullet_count < BULLET_LIMIT)
			{
				b.pos.y = enemies[i].pos.y + 20;
				b.pos.x = enemies[i].pos.x;
				b.speed = random(8, 12);
				if (enemies[i].pos.x < 120)
					b.direction = D_DOWN + random(0, 2);
				else
					b.direction = D_DOWN - random(0, 2);
				bullets[bullet_count] = b;
				bullet_count++;
			}
		}
		if (boss_fight)
		{
			if (bullet_count < BULLET_LIMIT)
				for (i = 0; i < 3 && bullet_count < BULLET_LIMIT; i++)
				{
					b.pos.y = boss.pos.y + 30;
					b.pos.x = boss.pos.x;
					b.speed = 10;
					b.direction = D_DOWN_2 + i;
					bullets[bullet_count] = b;
					bullet_count++;
				}
		}
	}
}

static void bullet_update()
{
	int i;
	Position new_pos;
	for (i = 0; i < bullet_count; i++)
	{
		new_pos = bullets[i].pos;
		switch (bullets[i].direction)
		{
			case D_UP_1: //-15
				new_pos.x -= (double) bullets[i].speed * 0.26;
				new_pos.y -= (double) bullets[i].speed * 0.966;
				break;
			case D_UP_2: //-30
				new_pos.x -= bullets[i].speed / 2;
				new_pos.y -= (double) bullets[i].speed * 1.732 / 2;
				break;
			case D_UP_3:
				new_pos.x += bullets[i].speed / 2;
				new_pos.y -= (double) bullets[i].speed * 1.732 / 2;
				break;
			case D_UP_4:
				new_pos.x += (double) bullets[i].speed * 0.26;
				new_pos.y -= (double) bullets[i].speed * 0.966;
				break;
			case D_UP:
				new_pos.y -= bullets[i].speed;
				break;
			case D_DOWN_1: //-45
				new_pos.x -= (double) bullets[i].speed * 0.26;
				new_pos.y += (double) bullets[i].speed * 0.966;
				break;
			case D_DOWN_2: //-30
				new_pos.x -= bullets[i].speed / 2;
				new_pos.y += (double) bullets[i].speed * 1.732 / 2;
				break;
			case D_DOWN:
				new_pos.y += bullets[i].speed;
				break;
			case D_DOWN_3: //30
				new_pos.x += bullets[i].speed / 2;
				new_pos.y += (double) bullets[i].speed * 1.732 / 2;
				break;
			case D_DOWN_4: //45
				new_pos.x += (double) bullets[i].speed * 0.26;
				new_pos.y += (double) bullets[i].speed * 0.966;
				break;
			default:
				break;
		}
		if (new_pos.y > 20 && new_pos.y < 310 && 
			new_pos.x > 10 && new_pos.x < 230)
		{
			draw_bullet(new_pos, bullets[i].pos, bullets[i].type, bullets[i].enemy ? LCD_COLOR_BLUE : LCD_COLOR_RED);
			bullets[i].pos = new_pos;
		}
		else
		{
			remove_bullet(i);
			i--;
		}
	}
}

void update_player(Direction d)
{
	Position new_pos;
	new_pos = player.pos;
	switch (d)
	{
		case D_LEFT:
			new_pos.x -= player.speed;
			if (player.pos.x < 20)
				player.pos.x = 20;
			break;
		case D_RIGHT:
			new_pos.x += player.speed;
			if (player.pos.x > 220)
				player.pos.x = 220;
			break;
		case D_UP:
			new_pos.y -= player.speed;
			if (player.pos.y < 30)
				player.pos.y = 30;
			break;
		case D_DOWN:
			new_pos.y += player.speed;
			if (player.pos.y > 300)
				player.pos.y = 300;
			break;
		default:
			break;
	}
	draw_jet(new_pos, player.pos, get_player_color());
	player.pos = new_pos;
}

static void game()
{
	Position bomb_pos = {10, 10};
	int record = 0;
	uint8_t bomb_flag = 0;
	clear();
	lcd_clear_screen(LCD_COLOR_BLACK);
	draw_jet(player.pos, player.pos, LCD_COLOR_RED);
	print_score();
	while (1)
	{
		if (alert)
		{
			alert = false;
			
			if (killed - 15 >= record)
			{
				int i;
				record = killed;
				player.hit_point = 30;
				draw_jet(player.pos, player.pos, get_player_color());
				if (enemy_spawn_time > 700)
					enemy_spawn_time -= 200;
				if (bomb < 8)
				{
					bomb++;
					bomb_flag = 0;
					bomb_pos.x = 10;
					for (i = 0; i < bomb; i++)
					{
						bomb_flag |= 1 << i;
						lcd_draw_image(bomb_pos.x, bombWidthPixels, bomb_pos.y, bombWidthPixels, bombBitmaps, LCD_COLOR_RED, LCD_COLOR_BLACK);
						bomb_pos.x += bombWidthPixels + 1;
					}
					set_led(bomb_flag);
				}
				if (record % 45 == 0)
				{
					boss_fight = true;
					boss_spawned = false;
				}
			}
			if (screen_touched() && bomb > 0)
			{
				int i;
				bomb -= 1;
				bomb_flag = 0;
				bomb_pos.x = 10;
				draw_rectangle_centered(60, 120, 8, 16, LCD_COLOR_BLACK);
				for (i = 0; i < bomb; i++)
				{
					bomb_flag |= 1 << i;
					lcd_draw_image(bomb_pos.x, bombWidthPixels, bomb_pos.y, bombWidthPixels, bombBitmaps, LCD_COLOR_RED, LCD_COLOR_BLACK);
					bomb_pos.x += bombWidthPixels + 1;
				}
				set_led(bomb_flag);
				for (i = 0; i < enemy_count; i++)
				{
					remove_jet(i);
					point += 100;
					killed += 1;
					i--;
				}
				if (boss_fight)
				{
					boss.hit_point -= 50;
					if (boss.hit_point <= 0)
						win_boss_fight();
				}
				print_score();
			}
			if (!(ft6x06_read_td_status() > 0))
				can_touch = true;
			
			if (enemy_alert)
			{
				enemy_alert = false;
				spawn_enemy();
			}
			bullet_update();
			enemy_update();
			
			if (collision() == 1)
			{
				draw_score_board();
				clear();
				return;
			}
			
			if (bullet_alert)
			{
				static int interval = 0;
				Bullet b = {{0, 0}, D_UP, 10, NORMAL, false};
				bullet_alert = false;
				interval++;
				if (bullet_count < BULLET_LIMIT)
				{
					b.pos.x = player.pos.x;
					b.pos.y = player.pos.y - 20;
					switch (bullet_type)
					{
						case NORMAL:
						{
							if (interval >= 3)
							{
								int i;
								interval = 0;
								for (i = 0; i < 2 && bullet_count < BULLET_LIMIT; i++)
								{
									b.direction = D_UP;
									b.pos.x = player.pos.x - 10 + i * 20;
									bullets[bullet_count] = b;
									bullet_count++;
								}
							}
							break;
						}
						case BIG:
						{
							if (interval >= 5)
							{
								interval = 0;
								b.type = BIG;
								bullets[bullet_count] = b;
								bullet_count++;
							}
							break;
						}
						case SCATTER:
						{
							if (interval >= 2)
							{
								int i;
								interval = 0;
								b.type = SCATTER;
								for (i = 0; i < 5 && bullet_count < BULLET_LIMIT; i++)
								{
									b.direction = D_UP_1 + i;
									bullets[bullet_count] = b;
									bullet_count++;
								}
							}
						}
					}
				}
				enemy_bullet();
			}
			
			if (alert_button)
			{
				switch (read_button())
				{
					case BUTTON_UP:
						bullet_type = NORMAL;
						break;
					case BUTTON_LEFT:
						bullet_type = BIG;
						break;
					case BUTTON_RIGHT:
						bullet_type = SCATTER;
						break;
					default:
						break;	
				}
			}
			
			if (ps2_direction != D_CENTER)
				update_player(ps2_direction);
			//else
			//	update_player(accel_direction(accel_x, accel_y));
		}
	}
}

void start()
{
	int x, y;
	//accel_x = accel_read_x();
	//accel_y = accel_read_y();
	draw_score_board();
	while (1)
	{
		if (screen_touched())
		{
			x = ft6x06_read_x();
			y = ft6x06_read_y();
			if (x >= 127 && x <= 240 && y >= 117 && y <= 203)
			{
				clear_score_board();
				draw_score_board();
			}
			else
				game();
		}
		if (!(ft6x06_read_td_status() > 0))
			can_touch = true;
	}
}