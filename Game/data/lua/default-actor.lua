-- Define all different Actors here.

-- add_component is registered to the actor factory class
-- add_component(actorId, componentName, componentData [optional])

default_actor = {
	TransformComponent = {
		pos_x = 0;
		pos_y = 60;
		pos_z = 0;

		scale = 10;
	},

	ScriptComponent = {
		
	},
	
	StaticModelComponent = {
		filename = "data/models/box.obj";
		--filename = "data/models/wizard/wizard_anims_split.x";
		alpha = 1.0;
	},

	on_created = function(self)
		x = math.random(-100, 100);
		y = math.random(0, 100);
		z = math.random(-100, 100);
		
		self:SetPosition(x, y, z);
		
		self.counter = 0.0;
		self.dir_x = 1;
		self.dir_alpha = -1;
		self.rotation_speed = math.random();
		
		set_camera_target(0, 0, 0);
		
		self.camera_dir = 1;
	end,
	
	on_update = function(self, dt)
		self.counter = self.counter + dt;
		
		if self.counter > 0.016 then
			
			
			-- Position
			pos = self:GetPosition();
			pos.x = pos.x + self.dir_x;
			self:SetPosition(pos.x, pos.y, pos.z);
			
			if pos.x > 100 then
				self.dir_x = -1;
			elseif pos.x < -100 then
				self.dir_x = 1;
			end
			
			-- Rotation
			rotation = self:GetRotation();
			rotation.y = rotation.y + self.rotation_speed/4;
			--self:SetRotation(rotation.x, rotation.y, rotation.z);
			
			-- Alpha
			local alpha = self:GetAlpha() + self.dir_alpha * 0.03;
			self:SetAlpha(alpha);
			
			if alpha < 0.1 then
				self.dir_alpha = 1;
			elseif alpha > 1 then
				self.dir_alpha = -1;
			end
			
			-- Camera
			self.counter = 0.0;
		end
		
		if key_pressed("VK_SPACE") then
			print("pressed");
			
			local pos = get_camera_pos();
			pos.y = pos.y - 30;
			set_camera_pos(pos.x, pos.y, pos.z);
			--set_camera_dir(1, -1, 0.1)
			--set_camera_target(20, 200, 0)
			
			--self:SetAnimationIndex(6, -1)
			--self:SetAnimationSpeed(180)
			--self:SetAlpha(math.random())
		end
	end,
	
	on_draw = function(self)
		--draw_string("Titta vad fint! :)", 400, 400, 60);
		
		local pos = self:GetPosition();
		draw_aabb(pos.x, pos.y, pos.z, 2, 1.0);
		
		--for i=1, 10, 1 do
		--	draw_texture("swag.bmp", 100 + i * 100, 600, 100, 100);
		--end
	end
}
