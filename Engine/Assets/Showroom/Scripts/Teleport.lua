local Teleport =
{
    destination = nil,
    toTeleport = nil
}

function Teleport:OnUpdate(deltaTime)
    if self.destination ~= nil then
        self.toTeleport:GetOwner():GetTransform():SetPosition(self.destination)
        Scenes.GetCurrentScene():FindActorByName("TeleportSource"):GetAudioSource():Play()
        self.destination = nil
    end
end

function Teleport:OnTriggerEnter(other)
    if other:GetOwner():GetTag() == "Player" then
        self.toTeleport = other
        if self.owner:GetTransform():GetPosition().y < 2 then
            self.destination = Vector3.new(-13, 5, -0.3)
        else
            self.destination = Vector3.new(-13, 1.25, -0.3)
        end
    end
end

return Teleport
