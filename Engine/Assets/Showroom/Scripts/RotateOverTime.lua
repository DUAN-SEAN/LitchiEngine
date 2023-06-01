local RotateOverTime =
{
    eulerAngles = Vector3.new(0, 0, 0)
}

function RotateOverTime:OnUpdate(deltaTime)
    self.eulerAngles.y = self.eulerAngles.y + 90 * deltaTime
    self.owner:GetTransform():SetRotation(Quaternion.new(self.eulerAngles))
end

function RotateOverTime:OnAwake()
    self.eulerAngles = self.owner:GetTransform():GetRotation():EulerAngles()
end

return RotateOverTime
