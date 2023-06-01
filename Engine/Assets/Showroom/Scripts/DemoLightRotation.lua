local DemoLightRotation =
{
    eulerAngles = Vector3.new(0, 0, 0)
}

function DemoLightRotation:OnAwake()
    self.eulerAngles = self.owner:GetTransform():GetRotation():EulerAngles()
end

function DemoLightRotation:OnUpdate(deltaTime)
    self.eulerAngles.y = self.eulerAngles.y + 20 * deltaTime
    self.owner:GetTransform():SetRotation(Quaternion.new(self.eulerAngles))
end

return DemoLightRotation
