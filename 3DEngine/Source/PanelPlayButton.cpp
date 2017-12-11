#include "PanelPlayButton.h"
#include "Application.h"

PanelPlayButton::PanelPlayButton()
{
	name = "PlayButton";
	active = true;
}

PanelPlayButton::~PanelPlayButton()
{
}

void PanelPlayButton::Draw()
{
	APPSTATE app_state = App->clock.state;

	ImGui::SetNextWindowPos(ImVec2((float)App->window->width*0.5f - 135.0f, 19.0f));
	ImGui::SetNextWindowSize(ImVec2(270.f, 30.0f));

	ImGui::Begin("Play Stop Tick", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	if (app_state == APP_PLAY || app_state == APP_PAUSE)
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 1.00f, 0.00f, 1.00f));
	if (ImGui::Button("Play", ImVec2(56.f, 18.f))) {
		if (app_state != APP_PLAY && app_state != APP_PAUSE) {
			App->clock.ChangeState(APP_PLAY);
			App->camera->ChangeCamera(false);
			// Serialize Scene before first play;
			App->scene_intro->SaveToPlay();
		}
		else {
			App->clock.ChangeState(APP_STOP);
			App->camera->ChangeCamera(true);
			// Load Serialized Scene after play
			App->scene_intro->LoadToStop();
		}
	}
	if (app_state == APP_PLAY || app_state == APP_PAUSE)
		ImGui::PopStyleColor();

	if (app_state == APP_PAUSE)
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 1.00f, 0.00f, 1.00f));
	ImGui::SameLine();
	if (ImGui::Button("Pause", ImVec2(56.f, 18.f))) {
		if (app_state == APP_PAUSE) {
			App->clock.ChangeState(APP_PLAY);
			App->camera->ChangeCamera(false);
		}
		else if (app_state == APP_PLAY) {
			App->clock.ChangeState(APP_PAUSE);
			App->camera->ChangeCamera(true);
		}
	}
	if (app_state == APP_PAUSE)
		ImGui::PopStyleColor();

	ImGui::SameLine();
	if (ImGui::Button("Tick", ImVec2(56.f, 18.f))) {
		App->clock.ChangeState(APP_TICK);
	}
	ImGui::SameLine();
	ImGui::Checkbox("Clock", &showclock);

	ImGui::End();

	if (showclock)
		DrawClock();

}

void PanelPlayButton::DrawClock()
{
	if (App->clock.state == APP_PLAY)
		ImGui::SetNextWindowPos(ImVec2((float)App->window->width - 300.0f, (float)App->window->height - 150.0f));
	else ImGui::SetNextWindowPos(ImVec2((float)App->window->width - 550.0f, (float)App->window->height - 360.0f));

	ImGui::SetNextWindowSize(ImVec2(300.f, 150.0f));

	ImGui::Begin("Clock Window", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	float time = (float)App->clock.real_time.Read() / 1000.f;
	ImGui::InputFloat("Time", &time, 0, 0, 2, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputInt("Frame", (int*)&App->clock.frame_count, 0, 0, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat("Game Time", &App->clock.game_time, 0, 0, 2, ImGuiInputTextFlags_ReadOnly);
	ImGui::SliderFloat("Time Scale", &App->clock.time_scale, 0.1f, 4.0f, "%.1f");
	ImGui::Separator();
	ImGui::InputFloat("dt", &App->clock.real_delta_time, 0, 0, 3, ImGuiInputTextFlags_ReadOnly);
	ImGui::InputFloat("Game dt", &App->clock.delta_time, 0, 0, 3, ImGuiInputTextFlags_ReadOnly);

	ImGui::End();
}