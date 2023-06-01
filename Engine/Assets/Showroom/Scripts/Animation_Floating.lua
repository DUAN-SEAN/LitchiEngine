local Animation_Floating =
{
    elasped = 0,
    startPos = Vector3.new(),
    frequency = 1.0,
    amplitude = 0.2
}

function Animation_Floating:OnAwake()
    self.startPos = self.owner:GetTransform():GetLocalPosition()
end

function Animation_Floating:OnUpdate(deltaTime)
    self.elasped = self.elasped + deltaTime
    self.owner:GetTransform():SetPosition(self.startPos + Vector3.new(0, math.sin(self.elasped * self.frequency) * self.amplitude, 0))
end

return Animation_Floating
