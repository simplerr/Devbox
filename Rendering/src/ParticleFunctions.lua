function InitParticle(pos_x, pos_y, pos_z, time)
	-- Create the particle
	local particle = Particle:new();

	-- Set attributes
	particle:SetInitialPos(pos_x, pos_y, pos_z);	-- Position
	particle:SetInitialTime(time);					-- Initial time
	particle:SetInitialSize(ParticleData.size);		-- Initial size
	particle:SetLifetime(ParticleData.lifetime);	-- Life time
	particle:SetMass(ParticleData.mass);			-- Mass
	particle:SetInitialColor(1, 0, 0, 1);			-- Initial color

	-- Velocity
	vel_x = math.random(-1000, 1000) / 1000.0 * ParticleData.speed;		-- X velocity
	vel_y = math.random(900, 1000) / 1000.0 * ParticleData.speed;		-- Y velocity
	vel_z = math.random(-1000, 1000) / 1000.0 * ParticleData.speed;		-- Z velocity
	particle:SetInitialVelocity(vel_x, vel_y, vel_z);

	particle:Debug();

	-- Return the new particle
	return particle;
end

function GetLifetime()
	return ParticleData.lifetime;
end

function GetTexture()
	return ParticleData.texture;
end