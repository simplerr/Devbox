-- Define all different Actors here.

-- add_component is registered to the actor factory class
-- add_component(actorId, componentName, componentData [optional])

player = {
	TransformComponent = {
		pos_x = 0;
		pos_y = 0;
		pos_z = 0;

		scale = 1;
	},

	ScriptComponent = {
	
	},
	
	--StaticModelComponent = {
	--	filename = "data/models/misc/TreeTrunk.obj";
	--	alpha = 1.0;
	--},

	on_created = function(self)
		local cameraPos = get_camera_pos();
		self:SetPosition(cameraPos.x, 200, cameraPos.z);
		
		self.counter = 0.0;
		self.dir_x = 1;
		self.dir_alpha = -1;
		self.rotation_speed = math.random();
		
		self.camera_dir = 1;

		self.y_speed = 0;
	end,
	
	-- The Player::Update() function
	on_update = function(self, dt)
		self.counter = self.counter + dt;

		-- Locals
		local playerPos = self:GetPosition();
		local cameraDir = get_camera_dir();
		local cameraRight = get_camera_right();

		-- Gravity physics
		local y_accel = -0.002;
		local max_y_speed = 0.4;

		self.y_speed = self.y_speed + y_accel;

		-- Limit the Y speed
		if self.y_speed < -max_y_speed then
			self.y_speed = -max_y_speed;
		elseif self.y_speed > max_y_speed then
			self.y_speed = max_y_speed;
		end
			

		-- Jumping
		if key_pressed("VK_SPACE") then
			self.y_speed = max_y_speed/2.5;	
		end

		local newY = playerPos.y + self.y_speed;
		
		-- Snap to the block terrain
		local current_block_height = get_block_height(playerPos.x, playerPos.y, playerPos.z);

		local units_above_ground = playerPos.y - (current_block_height*2);
		--print("units_above_ground ");
		--print(units_above_ground);

		if key_pressed("Z") then
			print("self.y_speed ");
			print(self.y_speed);
		end

		-- Only snap to the ground if self.y_speed < 0 and close to ground
		if self.y_speed < 0 and units_above_ground < 4 then
			newY = current_block_height*2 + 4;
			self.y_speed = 0.0;
		end

		-- Movement
		playerPos = self:GetPosition();
		local velocityX = 0; 
		local velocityZ = 0;
		local speed = 0.05;
		if key_down("W") then
			velocityX = cameraDir.x * speed;
			velocityZ = cameraDir.z * speed;
		end
		if key_down("S") then
			velocityX = -cameraDir.x * speed;
			velocityZ = -cameraDir.z * speed;
		end
		if key_down("A") then
			velocityX = -cameraRight.x * speed;
			velocityZ = -cameraRight.z * speed;
		end
		if key_down("D") then
			velocityX = cameraRight.x * speed;
			velocityZ = cameraRight.z * speed;
		end
			
		

		-- X
		local new_block_height = get_block_height(playerPos.x + velocityX, playerPos.y, playerPos.z);

		if key_pressed("X") then
			print("playerPos.y ");
			print(playerPos.y);
			print("\nnew_block_height ");
			print(new_block_height);
		end


		if new_block_height*2 + 4 > playerPos.y then
			velocityX = 0.0;
			--print("BLOCKED!");
		end

		-- Z
		new_block_height = get_block_height(playerPos.x, playerPos.y, playerPos.z + velocityZ);
		if new_block_height*2 + 4 > playerPos.y then
			velocityZ = 0.0;
			--print("BLOCKED!");
		end

		self:SetPosition(playerPos.x + velocityX, newY, playerPos.z + velocityZ);

		-- Camera
		self.counter = 0.0;

		-- Update the cameras position
		local newCameraPos = self:GetPosition();
		set_camera_pos(newCameraPos.x, newCameraPos.y, newCameraPos.z);
		set_camera_dir(cameraDir.x, cameraDir.y, cameraDir.z);
	end,
	
	on_draw = function(self)
		--draw_string("Titta vad fint! :)", 400, 400, 60);
		
		local pos = self:GetPosition();
		--draw_aabb(pos.x, pos.y, pos.z, 100, 1.0);
		
		--draw_aabb(10000, 100, 10000, 20, 1.0);

		--for i=1, 10, 1 do
		--	draw_texture("swag.bmp", 100 + i * 100, 600, 100, 100);
		--end
	end
}
