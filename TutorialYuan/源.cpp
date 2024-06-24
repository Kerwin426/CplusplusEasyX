#include<graphics.h>
#include<iostream>
#include<string>
#include<vector>


int idx_current_anim = 0;

const int PLAYER_ANIM_NUM = 6;

//IMAGE img_player_left[PLAYER_ANIM_NUM];
//IMAGE img_player_right[PLAYER_ANIM_NUM];



const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;




#pragma comment(lib,"Winmm.lib")
//��Ⱦ͸������putimage����Ⱦ����ɫ�߿�������Զ�����putimage_alpha
#pragma comment(lib,"MSIMG32.LIB")
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int h = img->getheight();
	int w = img->getwidth();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}
//��ͼƬ�ز�ȫ�����뵽IMAGE��ʽ��������,����һ���ü�������ʵ�ַ��������ǲ��ã������Բ������class animation���ع�����
//void LoadAnimation() {
//	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++) {
//		std::wstring path = L"img/paimon_left_" + std::to_wstring(i) + L".png";
//		loadimage(&img_player_left[i],path.c_str());
//	}
//	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++) {
//		std::wstring path = L"img/paimon_right_" + std::to_wstring(i) + L".png";
//		loadimage(&img_player_right[i], path.c_str());
//	}
//
//}


//������Ԫ��ģʽ��animation�������д�������ҵ�animation�е�һЩ��ͬ�㣬Ȼ����ȡ��atlas��
class Atlas {
public:
	Atlas(LPCTSTR path ,int num) {
		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++) {
			_stprintf_s(path_file, path, i);
			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame);
		}
	
	}
	~Atlas() {
		for (size_t i = 0; i < frame_list.size(); i++) {
			delete frame_list[i];
		}
	}

public:
	std::vector<IMAGE*> frame_list;
};

Atlas* atla_player_left;
Atlas* atla_player_right;
Atlas* atla_enemy_left;
Atlas* atla_enemy_right;
class Animation {
public:
	Animation(Atlas *atlas, int interval) {
		anim_atlas = atlas;
		interval_ms = interval;
	}
	~Animation() = default;
	//delta�Ǽ�ʱ��������һ�ε���play�������ж೤ʱ��
	void Play(int x, int y, int delta) {
		timer += delta;//��ʱ���ȼ������ã����������ֿ��˱��ٵĸо�
		if (timer >= interval_ms) {
			idx_frame = (idx_frame + 1) % anim_atlas->frame_list.size();
			timer = 0;
		}
		putimage_alpha(x, y, anim_atlas->frame_list[idx_frame]);
	}
private:
	int interval_ms = 0;
	int timer = 0;//��ʱ��
	int idx_frame = 0;//����֡����
	Atlas* anim_atlas;
};

class Player {
public:
	Player() {
		loadimage(&img_shadow, _T("img/shadow_player.png"));//������Ӱ��img_shadow��
		anim_left = new Animation(atla_player_left, 45);
		anim_right = new Animation(atla_player_right, 45);
	};
	~Player() {
		delete anim_left;
		delete anim_right;
	};

	void ProcessEvent(const ExMessage& msg) {
		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (msg.vkcode)
			{
			case VK_UP:
				is_move_up = true;
				break;
			case VK_DOWN:
				is_move_down = true;
				break;
			case VK_LEFT:
				is_move_left = true;
				break;
			case VK_RIGHT:
				is_move_right = true;
				break;
			}
			break;
		case WM_KEYUP:
			switch (msg.vkcode)
			{
			case VK_UP:
				is_move_up = false;
				break;
			case VK_DOWN:
				is_move_down = false;
				break;
			case VK_LEFT:
				is_move_left = false;
				break;
			case VK_RIGHT:
				is_move_right = false;
				break;
			}
			break;
		}

	}
	void Move() {
		//�ⲿ�ִ�����Ϊ��������Ӱ�һ����һ�㣬���ֻҪ�������ţ���һֱ��
		//����ԭ���ǰ�һ��+=speed�����״̬�Ŀ��ƣ�һֱ����up������is_move_up Ϊ��������ֱ�Ӽӣ�������ȥ�������Ĵ�����speed�ӵĴ�������
		//if (is_move_up) player_pos.y -= PLAYER_SPEED;
		//if(is_move_down)player_pos.y += PLAYER_SPEED;
		//if(is_move_left)player_pos.x -= PLAYER_SPEED;
		//if(is_move_right) player_pos.x += PLAYER_SPEED;

		//ͬʱ����xy��ķ�������ٶȻ��ɸ���2��������Ҫ����һ��
		int dir_x = is_move_right - is_move_left;
		int dir_y = is_move_down - is_move_up;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0) {
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			player_pos.x += (int)(PLAYER_SPEED * normalized_x);
			player_pos.y += (int)(PLAYER_SPEED * normalized_y);

		}
		//��paimon�����ڴ����ڲ�
		if (player_pos.x < 0)player_pos.x = 0;
		if (player_pos.y < 0)player_pos.y = 0;
		if ((player_pos.x + PLAYER_WIDTH) > WINDOW_WIDTH) player_pos.x = (WINDOW_WIDTH - PLAYER_WIDTH);
		if ((player_pos.y + PLAYER_HEIGHT) > WINDOW_HEIGHT) player_pos.y = (WINDOW_HEIGHT - PLAYER_HEIGHT);

	}
	void Draw(int delta) {
		int pos_shadow_x = player_pos.x + (PLAYER_WIDTH / 2 - SHADOW_WIDTH / 2);//����ӰͼƬ����
		int pos_shadow_y = player_pos.y + PLAYER_HEIGHT - 8;

		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);
		//��Ⱦ��Ӱ������drawplayer����Ϊ���������Ӱһͬ����

		int dir_x = is_move_right - is_move_left;
		static bool facing_left = false;
		if (dir_x < 0) {
			facing_left = true;
		}
		else if (dir_x > 0) {
			facing_left = false;
		}

		if (facing_left)
			anim_left->Play(player_pos.x, player_pos.y, delta);
		else
			anim_right->Play(player_pos.x, player_pos.y, delta);
	}
	const POINT& GetPosition()const {
		return player_pos;
	
	}
	const POINT& GetPlayerWH()const {
		return{PLAYER_WIDTH,PLAYER_HEIGHT};
	
	}
private:
	const int PLAYER_WIDTH = 80;
	const int PLAYER_HEIGHT = 80;
	const int SHADOW_WIDTH = 32;
	const int PLAYER_SPEED = 3;
	POINT player_pos = { 500,500 };
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
	IMAGE img_shadow;//������ӰͼƬ
	Animation* anim_left;
	Animation* anim_right;
};


class Bullet {
public:
	Bullet() = default;
	~Bullet() = default;
	POINT position = { 0,0 };
	void Draw() const {
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));
		fillcircle(position.x, position.y, RADIUS);
	}
private:
	const int RADIUS = 10;
};

class Enemy {
public:
	Enemy() {
		loadimage(&img_shadow, _T("img/shadow_enemy.png"));//������Ӱ��img_shadow��
		anim_left = new Animation(atla_enemy_left,45);
		anim_right = new Animation(atla_enemy_right,45);
		enum class SpawnEdge {
			Up = 0,
			Down,
			Left,
			Right

		};
		SpawnEdge edge = (SpawnEdge)(rand() % 4);
		switch (edge) {
		case SpawnEdge::Up:
			position.x = rand() % WINDOW_WIDTH;
			position.y = -FRAME_HEIGHT;
			break;
		case SpawnEdge::Down:
			position.x = rand() % WINDOW_WIDTH;
			position.y = WINDOW_HEIGHT;
			break;
		case SpawnEdge::Left:
			position.x = -FRAME_WIDTH;
			position.y = rand()%WINDOW_HEIGHT;
			break;
		case SpawnEdge::Right:
			position.x = WINDOW_WIDTH;
			position.y = rand()%WINDOW_HEIGHT;
			break;
		default:
			break;
		
		}
	}
	bool CheckBulletCollision(const Bullet& bullet) { 
		bool is_overlap_x = bullet.position.x >= position.x && bullet.position.x <= position.x + FRAME_WIDTH;
		bool is_overlap_y = bullet.position.y >= position.y && bullet.position.y <= position.y + FRAME_HEIGHT;
		return is_overlap_x&&is_overlap_y;
		
		}
	bool CheckPlayerCollision(const Player& player) { 
		
		POINT check_position = {position.x +FRAME_WIDTH/2 ,position.y +FRAME_HEIGHT /2};
		const POINT& player_position = player.GetPosition();
		bool is_overlap_x =check_position.x >= player_position.x&& check_position.x<=player_position.x+ player.GetPlayerWH().x;
		bool is_overlap_y = check_position.y >= player_position.y && check_position.y <= player_position.y + player.GetPlayerWH().y;
		return is_overlap_x&&is_overlap_y; 
	}
	void Move(const Player& player) {
		const POINT& player_position = player.GetPosition();
		int dir_x = player_position.x - position.x;
		int dir_y = player_position.y - position.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0) {
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(PLAYER_SPEED * normalized_x);
			position.y += (int)(PLAYER_SPEED * normalized_y);

		}
		if (dir_x < 0)
			facing_left = true;
		else if (dir_x > 0)
			facing_left = false;
	
	}
	void Draw(int delta) {
		int pos_shadow_x = position.x + (FRAME_WIDTH/2 -SHADOW_WIDTH/2);
		int pos_shadow_y = position.y + (FRAME_HEIGHT-35);
		putimage_alpha(pos_shadow_x,pos_shadow_y,&img_shadow);

		if (facing_left)
			anim_left->Play(position.x, position.y, delta);
		else
			anim_right->Play(position.x,position.y,delta);
	
	
	}
	void Hurt() {
		alive = false;
	}
	bool CheckAlive() {
		return alive;
	}
	~Enemy() {
		delete anim_left;
		delete anim_right;
	}
private:
	const int FRAME_WIDTH = 80;
	const int FRAME_HEIGHT = 80;//���˵Ŀ�Ⱥ͸߶�
	const int SHADOW_WIDTH = 48;
	const int PLAYER_SPEED = 2;
	IMAGE img_shadow;//������ӰͼƬ
	Animation* anim_left;
	Animation* anim_right;
	POINT position = { 0,0 };
	bool facing_left = false;
	bool alive = true;


};
//�������󣬽�ͼƬ���ص�IMAGE���������У�������play����ʱ������putimage_alpha��ȾͼƬ
//Animation anim_left_player(_T("img/paimon_left_%d.png"), 6, 10);
//Animation anim_right_player(_T("img/paimon_right_%d.png"), 6, 10);


IMAGE img_background;//��������

//delta
//void DrawPlayer(int delta, int dir_x) {
//}

void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)
{
	const int INTERVAL = 100;
	static int counter = 0;
	if ((++counter) % INTERVAL == 0) {
		enemy_list.push_back(new Enemy());
	}
}

void UPpdateBullets(std::vector<Bullet>& bullet_list, const Player& player) {
	const double RADIAL_SPEED = 0.0045;//�����ٶȣ������ӵ���Բ���ٶ�
	const double TANGENT_SPEED = 0.0055;//�����ٶȣ������ӵ��Ĳ����ٶ�
	double radian_interval = 2 * 3.1415926 / bullet_list.size();//�ӵ�֮��Ļ���
	
	POINT player_position = player.GetPosition();
	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);//��ʱ�����ӻ���
	for (size_t i = 0; i < bullet_list.size(); i++) {
		double radian = GetTickCount() * TANGENT_SPEED + radian_interval * i;
		bullet_list[i].position.x = player_position.x + player.GetPlayerWH().x / 2 + (int)(radius*sin(radian));
		bullet_list[i].position.y = player_position.y + player.GetPlayerWH().y / 2 + (int)(radius*cos(radian));
	
	}


}

void DrawPlayerScore(int score) {
	static TCHAR text[64];
	_stprintf_s(text,_T("��ǰ��ҵ÷�:%d"),score);

	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 85, 185));
	outtextxy(10,10,text);
}

int main() {
	initgraph(1280, 720);

	atla_player_left = new Atlas(_T("img/paimon_left_%d.png"), 6);
	atla_player_right = new Atlas(_T("img/paimon_right_%d.png"), 6);
	atla_enemy_left = new Atlas(_T("img/boar_left_%d.png"), 6);
	atla_enemy_right = new Atlas(_T("img/boar_right_%d.png"), 6);

	mciSendString(_T("open mus/bgm.mp3 alias bgm"),NULL,0,NULL);//mci�� media control interface ��һ�������Ǵ���windows��ָ��
	mciSendString(_T("open mus/hit.wav alias hit"),NULL,0,NULL);
	
	mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);

	bool running = true;

	ExMessage msg;

	Player player;

	int score=0;
	std::vector<Enemy*> enemy_list;

	std::vector<Bullet> bullet_list(3);

	loadimage(&img_background, _T("img/background.png"));//���ر�����img_background��




	//LoadAnimation();

	BeginBatchDraw();

	//DWORD begin_time = GetTickCount();
	while (running) {
		DWORD start_time = GetTickCount();

		while (peekmessage(&msg)) {

			player.ProcessEvent(msg);
		}
		player.Move();

		//ʵʱ�����ӵ�
		UPpdateBullets(bullet_list,player);

		//���ɵ��ˣ���ŵ�vector��
		TryGenerateEnemy(enemy_list);

		for (Enemy* enemy : enemy_list) {
			//ÿһ��enemy ��ִ��move����
			enemy->Move(player);
		}

		for (Enemy* enemy : enemy_list) {
			if (enemy->CheckPlayerCollision(player)) {
				static TCHAR text[128];
				_stprintf_s(text,_T("���յ÷�:%d"), score);
				MessageBox(GetHWnd(),text, _T("��Ϸ����"), MB_OK);
				running = false;
				break;
			}
		
		}
		for (Enemy* enemy : enemy_list) {
			for (const Bullet& bullet : bullet_list) {
				if (enemy->CheckBulletCollision(bullet)) {
					mciSendString(_T("play hit from 0"),NULL,0,NULL);
					enemy->Hurt();
					score++;
				}
			}
		}

		for (size_t i = 0; i < enemy_list.size(); i++) {
			Enemy* enemy = enemy_list[i];
			if (!enemy->CheckAlive()) {
				std::swap(enemy_list[i], enemy_list.back());
				enemy_list.pop_back();
				delete enemy;
			}
		}
		//static int counter = 0;
		//if (++counter % 5 == 0) idx_current_anim++;
		//idx_current_anim = idx_current_anim % PLAYER_ANIM_NUM;

		//DWORD current_time = GetTickCount();
		//int delta = (current_time - begin_time);


		cleardevice();

		putimage(0, 0, &img_background);

		player.Draw(1000 / 144);

		for (Enemy* enemy : enemy_list) {
			enemy->Draw(1000/144);
		}

		for (const Bullet& bullet : bullet_list) {
			bullet.Draw();
		}

		DrawPlayerScore(score);
		//DrawPlayer(1, is_move_right-is_move_left);
		//putimage_alpha(player_pos.x,player_pos.y, &img_player_left[idx_current_anim]);




		FlushBatchDraw();

		DWORD end_time = GetTickCount();

		DWORD delta_time = end_time - start_time;

		if (delta_time < 1000 / 144) {
			Sleep(1000 / 144 - delta_time);
		}

	}

	delete atla_enemy_left;
	delete atla_enemy_right;
	delete atla_player_left;
	delete atla_player_right;

	EndBatchDraw();
	return 0;
}